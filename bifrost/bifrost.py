"""
    bifrost.py
    ~~~~~~

    Bifrost, or sometimes Bilrost or Bivrost, is a burning rainbow bridge
    that reaches between Midgard (the world) and Asgard, the realm of the gods.
"""

import boto
import httplib
import urllib
import urllib2
import datetime
import markdown
import os
import shutil
import traceback
import logging
import pymongo
from os import environ
from uuid import uuid4
from itertools import chain


from flask import (
    Flask,
    abort,
    redirect,
    render_template,
    request,
    url_for,
    Markup,
)
from flask.ext.stormpath import (
    StormpathManager,
    User,
    login_required,
    login_user,
    logout_user,
    user,
)

from flask.ext.pagedown import PageDown
from flask.ext.pagedown.fields import PageDownField
from flask.ext.wtf import Form

from extensions.flask_stormpath import groups_allowed
from extensions.flask_stormpath import is_active_user_in
from extensions.sorted_by_name import sorted_by_name
from extensions.string_building import time_since_from_seconds
from extensions.string_building import timestamp_as_str

from pymongo import MongoClient
from pymongo.errors import PyMongoError

from stormpath.error import Error as StormpathError

from werkzeug.utils import secure_filename
from logging.handlers import TimedRotatingFileHandler

from wtforms.fields import BooleanField
from wtforms.fields import IntegerField
from wtforms.fields import HiddenField
from wtforms.fields import SelectField
from wtforms.fields import TextField
from wtforms.validators import DataRequired
from wtforms.validators import NumberRange
from wtforms.validators import Optional

##### Classes
## TODO: extract to another file
class ChallengeDescriptionForm(Form):
    """
    WTForm to submit a challenge. WTF was chosen here so we could use Flask-PageDown.
    """
    name = TextField('Challenge name', validators=[DataRequired()])
    visualizer = TextField('Visualizer file name', validators=[DataRequired()])
    thumbnail = TextField('Thumbnail URL', validators=[DataRequired()])
    dev_only = BooleanField('Dev only')
    description = PageDownField('Challenge description', validators=[DataRequired()])
    specs = PageDownField('General Technical Specs', validators=[DataRequired()])
    specs_cpp = PageDownField('C++ Technical Specs', validators=[DataRequired()])
    specs_cs = PageDownField('C# Technical Specs', validators=[DataRequired()])
    specs_java = PageDownField('Java Technical Specs', validators=[DataRequired()])
    specs_py = PageDownField('Python Technical Specs', validators=[DataRequired()])



class GroupDescriptionForm(Form):
    """
    WTForm to create a group.
    """
    name = TextField('Group name', validators=[DataRequired()])
    description = PageDownField('Group description', validators=[DataRequired()])
    admin_only = BooleanField('Admin only')

    # In some groups we want to maintain anonymity by showing only the usernames
    users_name_type = SelectField('Choose user name type', choices = [('username', 'Username'), ('full_name', 'Full name')])




class NewDescriptionForm(Form):
    """
    WTForm to create a news entry. WTF was chosen here so we could use Flask-PageDown.
    """
    title = TextField('Title', validators=[DataRequired()])
    pagedown = PageDownField('Content', validators=[DataRequired()])




class PlayForm(Form):
    """
    WTForm to specify a custom match
    """
    rounds = IntegerField('Rounds', validators=[NumberRange(min = 0, max = 20)])
    challenge = SelectField('Challenge')
    player = SelectField('Opponent', validators=[Optional()])
    form_type = HiddenField('Form type')




class TournamentForm(Form):
    """
    WTForm to specify a tournament
    """
    rounds = IntegerField('Rounds', validators=[NumberRange(min = 0, max = 20)])
    challenge = SelectField('Challenge')
    player1 = SelectField('First Player', validators=[Optional()])
    player2 = SelectField('Second Player', validators=[Optional()])
    all_play = BooleanField('All play')
    form_type = HiddenField('Form type')




##### Things to be exported to jinja
## TODO: extract to a configuration file?
def current_user_latest_matches():
    return latest_matches(uid = user.custom_data['uid'])

def latest_matches(uid = None, cid = None, limit = 8):
    query = {}

    if uid:
        query['users'] =  { '$elemMatch': { 'uid': uid } }

    if cid:
        query['cid'] = cid

    matches = list( mongodb.matches.find(query).sort([('datetime', -1)]).limit(limit) )
    challenges = list( mongodb.challenges.find({ 'cid': { '$in': [ match['cid'] for match in matches ] } }) )
    users = list( mongodb.users.find({ 'uid': { '$in': [ user['uid'] for user in chain.from_iterable(match['users'] for match in matches) ] } }) )
    username_for_given_uid = ([user['username'] for user in users if user['uid'] == uid] or [None])[0]
    # Maybe we should be using SQL :)

    latest = []

    for match in matches:
        match['challenge_name'] = next( challenge['name'] for challenge in challenges if challenge['cid'] == match['cid'] )
        match['usernames'] = [ user['username'] for user in users if user['uid'] in [match_user['uid'] for match_user in match['users']] ]
        match['opponents'] = [ username for username in match['usernames'] if username != username_for_given_uid ]

        time_delta = datetime.datetime.utcnow() - match['datetime']
        match['time_since'] = time_since_from_seconds( time_delta.total_seconds() )

        latest.append(match)

    return latest



# TODO: Fix Java name 
ACCEPTED_LANGUAGES = {'cs40': 'C# 4.0 (mono)',
                      'cpp11': 'C++11 (g++ 4.7.3)',
                      'python27': 'Python (2.7.3)',
                      'java7': 'Java (1.7)'}
ALL_PLAYERS = 'all-players-play'


##### Initialization
DEBUG = str(environ.get('MJOLLNIR_DEBUG')).lower()
DEBUG = (DEBUG == '1' or DEBUG == 'true')

app = Flask(__name__)

logger = logging.getLogger('werkzeug')
logger.setLevel(logging.INFO)
app.logger.setLevel(logging.INFO)

if DEBUG:
    BIFROST_LOG = '/Mjollnir/bifrost/logs/bifrost.log'

    # If log doesn't exist, create folder if necessary and create it
    if not os.path.isfile(BIFROST_LOG):
        if not os.path.isdir(os.path.dirname(BIFROST_LOG)):
            os.mkdir(os.path.dirname(BIFROST_LOG))
        open(BIFROST_LOG, "w") # Create file. No need to close since RefCount == 0

    handler = TimedRotatingFileHandler('/Mjollnir/bifrost/logs/bifrost.log', when='midnight', backupCount=7)
    logger.addHandler(handler)
    app.logger.addHandler(handler)


# Set environment variables
app.config['SECRET_KEY'] = environ.get('STORMPATH_SECRET_KEY')
app.config['STORMPATH_API_KEY_ID'] = environ.get('STORMPATH_API_KEY_ID')
app.config['STORMPATH_API_KEY_SECRET'] = environ.get('STORMPATH_API_KEY_SECRET')
app.config['STORMPATH_APPLICATION'] = environ.get('STORMPATH_APPLICATION')
app.config['MONGOLAB_URI'] = environ.get('MONGOLAB_URI')
app.config['MAX_CONTENT_LENGTH'] = 512 * 1024 # 512kB
app.config['YGG_BUILD_URL'] = environ.get('YGG_BUILD_URL')
app.config['YGG_PASSWORD'] = environ.get('YGG_PASSWORD')

# Export handy functions to jinja
def markdown_to_html(content):
    return Markup(markdown.markdown(content))

app.jinja_env.globals.update(format_exc = traceback.format_exc)
app.jinja_env.globals.update(is_active_user_in = is_active_user_in)
app.jinja_env.globals.update(len = len)
app.jinja_env.globals.update(markdown_to_html = markdown_to_html)
app.jinja_env.globals.update(latest_matches = latest_matches)
app.jinja_env.globals.update(current_user_latest_matches = current_user_latest_matches)
app.jinja_env.globals.update(ACCEPTED_LANGUAGES = ACCEPTED_LANGUAGES)
app.jinja_env.globals.update(enumerate = enumerate)
app.jinja_env.globals.update(timestamp_as_str = timestamp_as_str)
app.jinja_env.globals.update(MJOLLNIR_DEBUG = DEBUG)


# Stormpath
stormpath_manager = StormpathManager(app)
stormpath_manager.login_view = '.login'

# Mongodb
mongo_client = MongoClient(app.config['MONGOLAB_URI'])
mongodb = mongo_client['mjollnir-db']

# Amazon S3
if DEBUG:
    S3 = '/sandboxes/s3/'
    def upload_solution(siid, language, file):
        with open(S3 + 'solutions/' + siid + '.' + language, 'w') as f:
            f.write(file.read())

    @app.route('/mjollnir-matches/<mid>', methods=['GET'])
    def s3_mjollnir_matches(mid):
        filename = S3 + 'matches/' + mid

        if not os.path.isfile(filename):
            abort(404)

        file_content = ''
        with open(filename, 'r') as f:
            file_content = f.read()
        return file_content
else:
    s3 = boto.connect_s3()
    solutions_bucket = s3.get_bucket('mjollnir-solutions')

    def upload_solution(siid, language, file):
        key = solutions_bucket.new_key(siid)
        key.set_metadata('language', language)
        key.set_contents_from_file(file, headers=None, replace=True, cb=None, num_cb=10, policy=None, md5=None)

# Pagedown
pagedown = PageDown(app)


##### Website

### Error Handlers
@app.errorhandler(404)
def page_not_found(error):
    """
    Handler for HTTP 404.
    """
    return render_template('error.html', description = '404: Not Found', error = error), 404




@app.errorhandler(Exception)
def exception_handler(error):
    """
    Handler for HTTP 500 on unexpected exceptions.
    """
    strtime = datetime.datetime.now().strftime('%Y-%m-%d - %H:%M:%S')
    logger.info('====  ' + strtime + '  ====', )
    logger.info(traceback.format_exc(error))
    return render_template('error.html', description = "500: Internal Server Error", error = error), 500




### Webpage Handlers
@app.route('/')
def index():
    """
    Home page with Mjollnir news.
    Displays the latest 3 news.
    The HTTP parameter p specifies a specific page (skips 3*p news).
    """
    news_per_page = 3
    p = int(request.args.get('p', 0))

    news = list( mongodb.news.find().sort([('datetime', -1)]).skip(news_per_page*p).limit(news_per_page) )

    return render_template('news.html', custom_title = 'News', news = news, page = p)




@app.route('/newnew')
@groups_allowed(['Dev'])
def newnew():
    """ Page to add an entry to the news. """
    return redirect(url_for('.editnew'))

@app.route('/editnew', methods=['GET', 'POST'])
@groups_allowed(['Dev'])
def editnew():
    """
    Allows a user in the "Dev" admin group to create/edit a news entry.
    """
    nid = request.args.get('nid')
    existing_new = {}

    if nid:
        existing_new = mongodb.news.find_one({ 'nid': nid })
        if not existing_new:
            abort(404)

        date = existing_new['datetime']
    else:
        nid = str( uuid4() )
        date = datetime.datetime.utcnow()

    form = NewDescriptionForm(csrf_enabled = False)

    if request.method == 'GET':
        if existing_new:
            form.title.data = existing_new.get('title')
            form.pagedown.data = existing_new.get('content')

        return render_template('editnew.html', form = form)


    if form.validate_on_submit():
        document = { 'nid': nid,
                     'title': form.title.data,
                     'content': form.pagedown.data,
                     'datetime': date,
                     'author': user.username }

        if existing_new:
            mongodb.news.update({ 'nid': nid }, document)
        else:
            mongodb.news.insert(document)

        return redirect(url_for('.index'))

    else:
        return render_template('editnew.html', form = form, error = "Please enter all the required information."), 400




@app.route('/about')
def about():
    """
    About page.
    """
    return render_template('about.html')




@app.route('/register', methods=['GET', 'POST'])
def register():
    """
    This view allows a user to register for the site.
    """
    if request.method == 'GET':
        return render_template('register.html')

    try:
        # Create a new Stormpath User.
        _user = stormpath_manager.application.accounts.create({
            'email': request.form.get('email'),
            'username': request.form.get('username'),
            'password': request.form.get('password'),
            'given_name': request.form.get('given_name'),
            'surname': request.form.get('surname'),
        })
        _user.__class__ = User
    except StormpathError, err:
        return render_template('register.html', error=err.message), 400

    return redirect(url_for('.login', x='verifymail'))




@app.route('/login', methods=['GET', 'POST'])
def login():
    """
    This view logs in a user given an email address and password.
    Flask-Stormpath handles logins and user sessions.
    """
    if request.method == 'GET':
        return render_template('login.html')

    try:
        _user = User.from_login(
            request.form.get('email'),
            request.form.get('password'),
        )

        # Verify if the user is already in the database, if not we create a new entry
        # You might think doing this upon registration is a better idea, but we only want
        # to create database entries for users that confirmed their emails, and we need to
        # create entries before any page is displayed to the user.

        user_in_db = mongodb.users.find_one({ 'username': _user.username })

        if not user_in_db:
            # uid is generated here and not upon registering due to regression issues
            # TODO: change this after current users log in?
            uid = str(uuid4())

            mongodb.users.insert({
                'uid': uid,
                'username': _user.username,
                'email': _user.email,
                'given_name': _user.given_name,
                'surname': _user.surname
            })

            _user.custom_data['uid'] = uid
            _user.save()

        if not 'uid' in _user.custom_data:
            _user.custom_data['uid'] = user_in_db['uid']
            _user.save()

    except StormpathError, err:
        # TODO, some errors have wrong messages (e.g. 'incorrect password' upon unverified account)
        return render_template('login.html', error=err.message), 400

    login_user(_user, remember=True)
    return redirect(request.args.get('next') or url_for('dashboard'))




@app.route('/groups')
@login_required
def groups_dashboard():
    """
    Renders a list of groups the user can see
    """
    username = user.username
    if not username:
        abort(400)

    user_in_db = mongodb.users.find_one({ 'username': username })

    if not user_in_db:
        abort(404)

    groups = filter(lambda group: not group['admin_only'] or username in group['admins'] or is_active_user_in('Dev'), mongodb.groups.find())

    return render_template('groups.html', username = username, groups = groups)




# Might be better to use only /user/<username>
@app.route('/dashboard')
@login_required
def dashboard():
    return user_page(user.username)




@app.route('/user/<username>')
def user_page(username):
    """
    Renders a user's profile.
    """

    if not username:
        abort(400)
    user_in_db = mongodb.users.find_one({ 'username': username })
    if not user_in_db:
        abort(404)
    submissions = mongodb.submissions.find({ 'uid': user_in_db['uid'] })
    challenge_solutions = []

    for submission in submissions:
        challenge = mongodb.challenges.find_one({ 'cid': submission['cid'] })

        if not challenge:
            raise Exception("Could not find challenge " + submission['cid'] + " in the database")

        total_submissions = mongodb.submissions.find({ 'cid': submission['cid'] }).count()

        submission['name'] = challenge['name']
        submission['RD'] = int(round(submission['RD']))
        submission['rank'] = mongodb.submissions.find({ 'cid': submission['cid'], 'rating': { '$gt': submission['rating'] } }).count() + 1
        submission['percentile'] = round(100*float(total_submissions - submission['rank'] + 1)/total_submissions, 2)

        if not submission.get('build_description', ""):
            if submission['build_status'] == "Success":
                submission['build_description'] = "Your latest submission to this challenge was built successfully and is already in the queue to be executed."
            elif submission['build_status'] == "Waiting":
                submission['build_description'] = "Our servers are currently busy, but your submission will be built soon."
            else:
                submission['build_description'] = "Sorry, something went wrong. We couldn't build your code and have no error message in our logs."

        challenge_solutions.append(submission)

    return render_template('dashboard.html', user_in_db = user_in_db, challenge_solutions = challenge_solutions, custom_title = username)




@app.route('/logout')
@login_required
def logout():
    """
    Log out a logged in user.  Then redirect them back to the main page of the
    site.
    """
    logout_user()
    return redirect(url_for('index'))




#TODO: Add permission to Prof group
@app.route('/newgroup')
@groups_allowed(['Dev', 'Admin'])
def newgroup():
    return redirect(url_for('.editgroup'))




#TODO: enable csrf?
@app.route('/editgroup', methods=['GET', 'POST'])
@groups_allowed(['Dev', 'Admin'])
def editgroup():
    """
    Allows a user in the "Dev" admin group to create/edit groups
    """
    group_id = request.args.get('gid')
    group = {}
    username = user.username

    if group_id:
        group = mongodb.groups.find_one({ 'gid': group_id })
        if not group:
            abort(404)
    else:
        group_id = str( uuid4() )

    form = GroupDescriptionForm(csrf_enabled = False)

    if request.method == 'GET':
        if group:
            form.name.data = group.get('name')
            form.admin_only.data = group.get('admin_only')
            form.description.data = group.get('description')
            form.users_name_type.data = group.get('users_name_type')
        else:
            form.admin_only.data = True

        return render_template('editgroup.html', form = form)

    if form.validate_on_submit():
        document = { 'gid': group_id,
                     'name': form.name.data,
                     'admin_only': form.admin_only.data,
                     'description': form.description.data,
                     'users_name_type': form.users_name_type.data
                   }

        if group:
            document['users'] = group['users']
            document['admins'] = group['admins']
            mongodb.groups.update({ 'gid': group_id }, document)

        else:
            document['users'] = [username]
            document['admins'] = [username]
            mongodb.groups.insert(document)

        return redirect(url_for('.group', gid = group_id))

    else:
        return render_template('editgroup.html', form = form, error = "Please enter all the required information."), 400




@app.route('/tournament/<tid>')
def tournament(tid):
    """
    Page to visualize a tournament.
    """
    tournament = mongodb.tournaments.find_one({ 'tid': tid })
    if not tournament:
        abort(404)

    group = mongodb.groups.find_one({'gid': tournament['gid']})
    if not group:
        error = "The group where this tournament was held no longer exists"

        return render_template('tournament.html', error = error)

    matches = list( mongodb.matches.find({ 'tid': tid }) )

    names_dict = dict()
    for match in matches:

        for idx in range(len(match['users'])):
            
            uid = match['users'][idx]['uid']
            
            if uid not in names_dict:
                
                user = mongodb.users.find_one({'uid': uid})
                names_dict[uid] = dict()
                if 'given_name' in user and 'surname' in user:
                    names_dict[uid]['full_name'] = user['given_name'] + ' ' + user['surname']
                else: 
                    names_dict[uid]['full_name'] = user['username']
                names_dict[uid]['username'] = user['username']

            match['users'][idx]['username'] = names_dict[uid]['username']
            match['users'][idx]['full_name'] = names_dict[uid]['full_name']

        counter = 1
        if 'errors' in match:
            for error in match['errors']:
                match['error_message'] = ''
                if error == 'server':
                    match['error_message'] = str(counter) + '. There was a problem in the server. '
                    counter = counter + 1
                else:
                    match['error_message'] = match['error_message'] + str(counter) + '. There was a problem with ' + names_dict[error]['username'] + '\'s submission. '  
        else:
            if len(match['users']) == 1:
                winner = match['users'][0]['rank']
            else:
                winner = dict()
                if match['users'][0]['rank'] == 1 and match['users'][1]['rank'] == 2:
                    winner['username'] = match['users'][0]['username']
                    winner['full_name'] = match['users'][0]['username']

                elif match['users'][1]['rank'] == 1 and match['users'][0]['rank'] == 2:
                    winner['username'] = match['users'][1]['username']
                    winner['full_name'] = match['users'][1]['username']
                else:
                    winner['username'] = 'Tie!'
                    winner['full_name'] = 'Tie!'

            match['winner'] = winner


    if len(matches) < tournament['total_matches']:
    
        return render_template('tournament.html', tournament = tournament, matches = matches, names_type = group['users_name_type'])
    
    if 'ranking' not in tournament:
        ranking = make_ranking(matches, tournament, group, names_dict)
        mongodb.tournaments.update({'tid': tid}, {'$set': {'ranking': ranking}})  
        tournament['ranking'] = ranking      

    return render_template('tournament.html', tournament = tournament, matches = matches, names_type = group['users_name_type'])




def make_ranking(matches, tournament, group, names_dict):

        d = dict()
        ranking = list()
        for match in matches:
            if tournament['challenge_name'] == 'Wumpus':
                uid = match['users'][0]['uid'] 
                score = match['users'][0]['rank']
                if uid in d:
                    d[uid].append(score)
                else:
                    d[uid] = [score]
            else:
                uid1 = match['users'][0]['uid']
                uid2 = match['users'][1]['uid']
                rank1 = match['users'][0]['rank']
                rank2 = match['users'][1]['rank']
                if tournament['challenge_name'] == 'Backgammon':
                    if rank1 == 1 and rank2 == 2:
                        score1 = 1
                        score2 = 0
                    else:
                        score1 = 0
                        score2 = 1
                else:
                    if rank1 == 1 and rank2 == 2:
                        score1 = 3
                        score2 = 0
                    elif rank2 == 1 and rank1 == 2:
                        score1 = 0
                        score2 = 3
                    else:
                        score1 = 1
                        score2 = 1
                if uid1 in d:
                    d[uid1].append(score1)
                else:
                    d[uid1] = [score1]
                if uid2 in d:
                    d[uid2].append(score2)
                else:
                    d[uid2] = [score2]

        if tournament['challenge_name'] == 'Wumpus':
            for uid in d:
                total_score = 0
                rounds = len(d[uid])
                for score in d[uid]:
                    total_score = total_score + score
                total_score = total_score / rounds
                ranking.append([uid, [total_score]])
        else:
           for uid in d:
                total_score = 0
                matches_won = 0
                matches_tie = 0
                for score in d[uid]:
                    total_score = total_score + score
                    if tournament['challenge_name'] == 'Backgammon':
                        if score == 1:
                            matches_won = matches_won + 1
                    else:
                        if score == 3:
                            matches_won = matches_won + 1
                        elif score == 1:
                            matches_tie = matches_tie + 1
                ranking.append([uid, [total_score, matches_won, matches_tie]])

        ranking.sort(key = lambda x: x[1][0], reverse = True)

        for row in ranking:

            uid = row[0]
            row[0] = dict()
            row[0] = names_dict[uid]

        return ranking




@app.route('/group/<gid>', methods=['GET', 'POST'])
@login_required
def group(gid):
    """
    Page to display a group given a gid.
    """
    group = mongodb.groups.find_one({'gid': gid})
    if not group or ( user.username not in group['admins'] and group['admin_only'] ):
        abort(404)

    playForm = PlayForm(csrf_enabled = False, prefix = 'playForm')
    tournamentForm = TournamentForm(csrf_enabled = False, prefix = 'tournamentForm') 
    
    user_id = user.custom_data['uid']

    group_users = list()
    for player in group['users']:
        user_in_db = mongodb.users.find_one({ 'username': player})
        uid = user_in_db['uid']
        name = player

        if group['users_name_type'] == 'full_name':
            # In case the user don't have a full name (old users)
            if 'given_name' in user_in_db and 'surname' in user_in_db:
                name = user_in_db['given_name'] + ' ' + user_in_db['surname']

        # We are considering that players can't play against themselves
        if uid != user_id:
            group_users.append((uid, name))

    limit = 5
    tournaments = list( mongodb.tournaments.find({ 'gid': group['gid']}).sort('datetime_started', pymongo.DESCENDING).limit(limit) )

    challenges = list( mongodb.challenges.find({}) )
    challenges_choices = [(challenge['cid'], challenge['name']) for challenge in challenges]

    for tournament in tournaments:
        time_delta = datetime.datetime.utcnow() - tournament['datetime_started']
        tournament['time_since'] = time_since_from_seconds( time_delta.total_seconds() )

    # Populating playForm drop down lists
    playForm.challenge.choices = challenges_choices
    playForm.player.choices = group_users

    # Populating tournamentForm drop down lists
    tournamentForm.challenge.choices = challenges_choices
    tournamentForm.player1.choices = group_users
    tournamentForm.player2.choices = group_users

    if request.method == 'GET':
        playForm.rounds.data = 1

        tournamentForm.rounds.data = 1
        tournamentForm.all_play.data = False

        return render_template('group.html', group = group, playForm = playForm, tournamentForm = tournamentForm, tournaments = tournaments)

    if playForm.is_submitted() and playForm.form_type.data == "play":
        if playForm.validate():
            rounds = playForm.rounds.data
            cid = playForm.challenge.data
            opponent = playForm.player.data
            challenge = mongodb.challenges.find_one({'cid': cid})

            if challenge['name'] == 'Wumpus':
                error = play(cid = cid, uids = [user_id], rounds = rounds)
            else:
                error = play(cid = cid, uids = [user_id, opponent], rounds = rounds)

            if error:
                return render_template('group.html', group = group, playForm = playForm, tournamentForm = tournamentForm, error = error)

            return redirect(url_for('.matches'))
        else:
            field, errors = playForm.errors.items()[0]
            error = playForm[field].label.text + ': ' + ', '.join(errors)
            return render_template('group.html', group = group, playForm = playForm, tournamentForm = tournamentForm, tournaments = tournament, error = error)


    if tournamentForm.is_submitted() and tournamentForm.form_type.data == 'tournament':
        if tournamentForm.validate():
            rounds = tournamentForm.rounds.data
            cid = tournamentForm.challenge.data
            player1 = tournamentForm.player1.data
            player2 = tournamentForm.player2.data
            all_play = tournamentForm.all_play.data
            challenge = mongodb.challenges.find_one({'cid': cid})

            if all_play:

                if challenge['name'] != 'Wumpus' and len(group['users']) < 2:
                    error = 'This group needs at least one more user to held a tournament with this challenge'
                    return render_template('group.html', group = group, playForm = playForm, tournamentForm = tournamentForm, error = error)

                allPlay(cid = cid, rounds = rounds, group = group, challenge_name = challenge['name'])
                return redirect(url_for('.matches'))

            if challenge['name'] == 'Wumpus':
                error = play(cid = cid, uids = [player1], rounds = rounds)
            else:
                error = play(cid = cid, uids = [player1, player2], rounds = rounds)

            if error:
                return render_template('group.html', group = group, playForm = playForm, tournamentForm = tournamentForm, error = error)

            return redirect(url_for('.matches'))
        else:
            field, errors = tournamentForm.errors.items()[0]
            error = tournamentForm[field].label.text + ': ' + ', '.join(errors)
            return render_template('group.html', group = group, playForm = playForm, tournamentForm = tournamentForm, tournaments = tournaments, error = error)

    # Should never reach this line, since either the method is GET, either it's POST.
    # If it's POST, then either one of the forms has been submitted.
    raise Exception("Should never reach this line")




@app.route('/newchallenge')
@groups_allowed(['Dev'])
def newchallenge():
    return redirect(url_for('.editchallenge'))

@app.route('/editchallenge', methods=['GET', 'POST'])
@groups_allowed(['Dev'])
def editchallenge():
    """
    Allows a user in the "Dev" admin group to create/edit a challenge.
    """
    challenge_id = request.args.get('cid')
    challenge = {}

    if challenge_id:
        challenge = mongodb.challenges.find_one({ 'cid': challenge_id })
        if not challenge:
            abort(404)
    else:
        challenge_id = str( uuid4() )

    form = ChallengeDescriptionForm(csrf_enabled = False)

    if request.method == 'GET':
        if challenge:
            form.name.data = challenge.get('name')
            form.visualizer.data = challenge.get('visualizer')
            form.thumbnail.data = challenge.get('thumbnail')
            form.dev_only.data = challenge.get('dev_only')
            form.description.data = challenge.get('description')
            form.specs.data = challenge.get('specs')
            form.specs_cpp.data = challenge.get('specs_cpp')
            form.specs_cs.data = challenge.get('specs_cs')
            form.specs_java.data = challenge.get('specs_java')
            form.specs_py.data = challenge.get('specs_py')
        else:
            form.dev_only.data = True

        return render_template('editchallenge.html', form = form)


    if form.validate_on_submit():
        document = { 'cid': challenge_id,
                     'name': form.name.data,
                     'visualizer': form.visualizer.data,
                     'thumbnail': form.thumbnail.data,
                     'description': form.description.data,
                     'specs': form.specs.data,
                     'specs_cpp': form.specs_cpp.data,
                     'specs_cs': form.specs_cs.data,
                     'specs_java': form.specs_java.data,
                     'specs_py': form.specs_py.data,
                     'dev_only': form.dev_only.data }

        if challenge:
            mongodb.challenges.update({ 'cid': challenge_id }, document)
        else:
            mongodb.challenges.insert(document)

        return redirect(url_for('.challenge_by_name', challenge_name = form.name.data))

    else:
        return render_template('editchallenge.html', form = form, error = "Please enter all the required information."), 400




@app.route('/challenge/<challenge_name>')
def challenge_by_name(challenge_name):
    """
    Page to display a challenge given a problem name.
    """
    challenge = mongodb.challenges.find_one({'name': challenge_name})

    if not challenge or ( not is_active_user_in('Dev') and challenge['dev_only'] ):
        abort(404)

    submissions = mongodb.submissions.find({ 'cid': challenge['cid'] }).sort([ ('rating', -1) ]).limit(10)
    challenge_solutions = []

    i = 0
    for submission in submissions:
        i += 1
        # TODO: Batch request
        user_from_submission = mongodb.users.find_one({ 'uid': submission['uid'] })

        if not user_from_submission:
            raise Exception("Could not find user " + submission['uid'] + " in the database")

        submission['sequence'] = i
        submission['username'] = user_from_submission['username']
        submission['RD'] = int(round(submission['RD']))
        challenge_solutions.append(submission)

    matches = latest_matches( cid = challenge['cid'] )

    return render_template('challenge.html', challenge = challenge, challenge_solutions = challenge_solutions, matches = matches, custom_title = challenge_name)




@app.route('/challenge')
def challenge():
    """
    Page to display a challenge given an id.
    """
    challenge_id = request.args.get('cid')

    if not challenge_id:
        abort(404)

    challenge = mongodb.challenges.find_one({'cid': challenge_id})

    if challenge and ( is_active_user_in('Dev') or not challenge['dev_only'] ):
        return render_template('challenge.html', challenge=challenge)
    else:
        abort(404)




@app.route('/challenges')
def challenges():
    """
    Page to display all challenges
    """
    challenges = sorted_by_name( [challenge for challenge in mongodb.challenges.find() if ( not challenge['dev_only'] or is_active_user_in('Dev') )] )
    return render_template('challenges.html', challenges=challenges)




@app.route('/join/<gid>', methods=['GET', 'POST'])
@login_required
def joingroup(gid):
    """
    Allows a user to join/leave a group
    """
    username = user.username
    if not username:
        abort(400)

    group = mongodb.groups.find_one({ 'gid': gid })
    if not group:
        abort(404)

    if username not in group['users']:
        mongodb.groups.update(
            { 'gid': gid },
            { '$push':
                {
                    'users': username
                }
            }
        )
    else:
        mongodb.groups.update(
            { 'gid': gid },
            { '$pull':
                {
                    'users': username
                }
            }
        )

    groups = list( mongodb.groups.find() )

    if groups:
        for group in groups:
            if group['admin_only'] and username not in group['admins']:
                groups.remove(group)

    return render_template('groups.html', username = username, groups = groups)




@app.route('/submit/<challenge_name>', methods=['GET', 'POST'])
@login_required
def submitsolution(challenge_name):
    """
    Allows a user to submit/update (override) a solution to an existing challenge.
    """
    challenge = mongodb.challenges.find_one({'name': challenge_name})

    if not challenge or ( not is_active_user_in('Dev') and challenge['dev_only'] ):
        abort(404)

    if request.method == 'GET':
        return render_template('submitsolution.html', challenge = challenge)

    if 'language' not in request.form:
        return render_template('submit.solution.html', challenge = challenge, error = "Please select a language"), 400

    language = request.form['language']
    if language not in ACCEPTED_LANGUAGES.keys():
        return render_template('submitsolution.html', challenge = challenge, error = "Invalid language"), 403

    file = request.files['sourcefile']
    if not file:
        return render_template('submitsolution.html', challenge = challenge, error = "Please select a Source File"), 400

    # Source/Solution/Submission (you choose!) Instance ID
    siid = str(uuid4())

    # Upload the source file to the 'mjollnir-solutions' S3 bucket using the siid as the key
    upload_solution(siid, language, file)

    # Update/Create a database entry for this submission
    query_existing_solution = { 'uid': user.custom_data['uid'], 'cid': challenge['cid'] }
    existing_solution = mongodb.submissions.find_one(query_existing_solution)

    if existing_solution:
        # For an existing solution, we have to set the 'build_' attributes and notify
        #   the compiler service. The service is responsible for updating the database entries
        #   when it finishes compiling.

        update_document = { '$set': { 'build_siid': siid,
                                      'build_status': "Waiting",
                                      'build_description': "" } }

        mongodb.submissions.update(query_existing_solution, update_document)

        document = existing_solution

    else:
        # For new solutions, we just add the document blueprint

        document = { 'siid': '',
                     'build_siid': siid,
                     'build_status': "Waiting",
                     'build_description': "",
                     'cid': challenge['cid'],
                     'uid': user.custom_data['uid'],
                     'sid': str(uuid4()),
                     'rating': 1500,
                     'RD': 300.0,
                     'previous_submissions': [] }

        mongodb.submissions.insert(document)

    # Send to build
    data = urllib.urlencode({'sid': document['sid'], 'cid': document['cid'], 'password': app.config['YGG_PASSWORD']})
    headers = {'Content-type': 'application/x-www-form-urlencoded', 'Accept': 'text/plain'}
    conn = httplib.HTTPConnection(app.config['YGG_BUILD_URL'])
    conn.request('POST', '/build', data, headers)
    response = conn.getresponse()
    conn.close()

    if response.status >= 300:
        raise Exception("Invalid Yggdrasil status code: " + str(response.status) + "\n" + response.read())

    return redirect(url_for('dashboard'))




@app.route('/match/<mid>')
def match(mid):
    """
    Page to visualize a match.
    """
    match = mongodb.matches.find_one({ 'mid': mid })
    if not match:
        abort(404)

    challenge = mongodb.challenges.find_one({ 'cid': match['cid'] })

    if not challenge:
        raise Exception("Couldn't find a challenge with cid " + match['cid'])

    users = list(mongodb.users.find({'uid': {'$in': [user['uid'] for user in match['users']]}}))
    users_dict = {user['uid']: user for user in users}

    time_delta = datetime.datetime.utcnow() - match['datetime']
    match['time_since'] = time_since_from_seconds(time_delta.total_seconds())
    match['challenge_name'] = challenge['name']
    match['visualizer'] = challenge['visualizer']
    match['usernames'] = [users_dict[user['uid']]['username'] for user in match['users']]

    if match['challenge_name'] == 'Wumpus':
        match['winner'] = 'Score: ' + str(match['users'][0]['rank'])
    else:
        if match['users'][0]['rank'] == 1 and match['users'][1]['rank'] == 2:
            match['winner'] = 'Winner: ' + match['usernames'][0]
        elif match['users'][0]['rank'] == 2 and match['users'][1]['rank'] == 1:
            match['winner'] = 'Winner: ' + match['usernames'][1]
        else: 
            match['winner'] = 'Tie!'

    match['users'] = users

    # Checking for errors
    if 'errors' in match:
        errors = match['errors']
        culprits = []
        if 'server' in errors:
            culprits.append('on the server')
            errors.remove('server')
        culprits.extend([('on the %s solution' % users_dict[uid]['username']) for uid in errors])
        if len(culprits) > 1:
            s = ', '.join(culprits[:-1]) + ' and ' + culprits[-1]
        else:
            s = culprits[0]
        match['error_message'] = 'There was an error %s during this match.' % s
    else:
        match['error_message'] = None

    custom_title = ' vs '.join(match['usernames']) + ' on ' + match['challenge_name']

    return render_template('match.html', match = match, custom_title = custom_title)




@app.route('/matches')
def matches():
    """
    Page to display the 10 most recent matches.
    """
    matches = latest_matches(limit = 10)
    return render_template('matches.html', matches = matches)




def allPlay(cid, rounds, group, challenge_name):
    """
    Make all players play
    Currently working with games for one and two players
    """

    tid = str( uuid4() )

    playing = 0

    users = [mongodb.users.find_one({ 'username': username }) for username in group['users']]

    if challenge_name == 'Wumpus':
        for user in users:
            error = play(cid = cid, uids = [user['uid']], rounds = rounds, tid = tid) 
            if not error:
                playing = playing + 1

    else:
        for i in xrange(len(users) - 1):
            for j in xrange(i + 1, len(users)):
                error = play(cid = cid, uids = [users[i]['uid'], users[j]['uid']], rounds = rounds, tid = tid)
                if not error:
                    playing = playing + 1


    total_matches = rounds * playing    
    document = {
        'tid': tid,
        'cid': cid,
        'challenge_name': challenge_name,
        'gid': group['gid'],
        'total_matches': total_matches,
        'datetime_started': datetime.datetime.utcnow(),
    }
    mongodb.tournaments.insert(document)



def play(cid, uids, rounds, tid = None):
    subs = list()

    for uid in uids:
        sub = mongodb.submissions.find_one({ 'uid': uid, 'cid': cid })

        if not sub:
            return "No submission found for one of the players"

        # TODO: We should use a previous submission if we can
        if sub['build_status'] != 'Success':
            return "One of the submissions haven't built properly"

        subs.append(sub)

    values = {  'cid': cid,
                'siids': [sub['siid'] for sub in subs],
                'uids': uids,
                'password': app.config['YGG_PASSWORD'] }
    if tid:
        values['tid'] = tid

    encoded = urllib.urlencode(values)

    for _ in xrange(rounds):
        headers = {'Content-type': 'application/x-www-form-urlencoded', 'Accept': 'text/plain'}
        conn = httplib.HTTPConnection(app.config['YGG_BUILD_URL'])
        conn.request('POST', '/run', encoded, headers)
        response = conn.getresponse()
        conn.close()

    return False




@app.route('/pleaseMakeCoffee', methods=['BREW', 'POST', 'GET'])
def teapot():
    """
    Allows a user to ask for a cup of coffee.
    """
    abort(418)



# On unix systems the project should be executed using Gunicorn and Foreman.
# Since Gunicorn doesn't run in windows yet, we let Flask itself handle the requests on nt systems.
if os.name == 'nt':
    if __name__ == "__main__":
        app.run(host='0.0.0.0', port=8080, debug=True)
else:
    print "Try 'foreman start'"
