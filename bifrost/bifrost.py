"""
    bifrost.py
    ~~~~~~

    Bifrost, or sometimes Bilrost or Bivrost, is a burning rainbow bridge
    that reaches between Midgard (the world) and Asgard, the realm of the gods.
"""

import boto
import datetime
import markdown
import os
import traceback
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

from wtforms.fields import TextField
from wtforms.fields import BooleanField
from wtforms.validators import DataRequired




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
    specs_py = PageDownField('Python Technical Specs', validators=[DataRequired()])




class NewDescriptionForm(Form):
    """
    WTForm to create a news entry. WTF was chosen here so we could use Flask-PageDown.
    """
    title = TextField('Title', validators=[DataRequired()])
    pagedown = PageDownField('Content', validators=[DataRequired()])




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




ACCEPTED_LANGUAGES = {'cs40': 'C# 4.0 (mono)',
                      'cpp11': 'C++11 (g++ 4.7.3)',
                      'python27': 'Python (2.7.3)'}




##### Initialization
app = Flask(__name__)

# Set environment variables
app.config['SECRET_KEY'] = environ.get('STORMPATH_SECRET_KEY')
app.config['STORMPATH_API_KEY_ID'] = environ.get('STORMPATH_API_KEY_ID')
app.config['STORMPATH_API_KEY_SECRET'] = environ.get('STORMPATH_API_KEY_SECRET')
app.config['STORMPATH_APPLICATION'] = environ.get('STORMPATH_APPLICATION')
app.config['MONGOLAB_URI'] = environ.get('MONGOLAB_URI')
app.config['MAX_CONTENT_LENGTH'] = 512 * 1024 # 512kB

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

# Stormpath
stormpath_manager = StormpathManager(app)
stormpath_manager.login_view = '.login'

# Mongodb
mongo_client = MongoClient(app.config['MONGOLAB_URI'])
mongodb = mongo_client['mjollnir-db']

# Amazon S3
s3 = boto.connect_s3()

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
    return render_template('error.html', description = '500: Internal Server Error', error = error), 500




### Webpage Handlers
@app.route('/')
def index():
    """
        Home page with Mjollnir news.
        Displays the latest 3 news.
        The HTTP parameter p specifies a specific page (skips 3*p news).
    """
    news_per_page = 3
    p = request.args.get('p', 0)

    news = list( mongodb.news.find().sort([('datetime', -1)]).skip(news_per_page*p).limit(news_per_page) )

    return render_template('news.html', custom_title = "News", news = news, page = p)




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
    """ About page. """
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
            'given_name': request.form.get('username'),
            'surname': 'Xupa Croata',
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
                'email': _user.email
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
            raise "Could not find challenge " + submission['cid'] + " in the database"

        total_submissions = mongodb.submissions.find({ 'cid': submission['cid'] }).count()

        submission['name'] = challenge['name']
        submission['RD'] = round(submission['RD'], 2)
        submission['rank'] = mongodb.submissions.find({ 'cid': submission['cid'], 'rating': { '$gt': submission['rating'] } }).count() + 1
        submission['percentile'] = round(100*float(total_submissions - submission['rank'] + 1)/total_submissions, 2)
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
    challenge = mongodb.challenges.find_one({"name": challenge_name})

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
            raise "Could not find user " + submission['uid'] + " in the database"

        submission['sequence'] = i
        submission['username'] = user_from_submission['username']
        submission['RD'] = round(submission['RD'], 2)
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

    challenge = mongodb.challenges.find_one({"cid": challenge_id})

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




@app.route('/submit/<challenge_name>', methods=['GET', 'POST'])
@login_required
def submitsolution(challenge_name):
    """
    Allows a user to submit/update (override) a solution to an existing challenge.
    """
    challenge = mongodb.challenges.find_one({"name": challenge_name})

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
    filename = secure_filename(file.filename)
    solutions_bucket = s3.get_bucket('mjollnir-solutions')
    key = solutions_bucket.new_key(siid)
    key.set_metadata('language', language)
    key.set_contents_from_file(file, headers=None, replace=True, cb=None, num_cb=10, policy=None, md5=None) 

    # Update/Create a database entry for this submission
    query_existing_solution = { 'uid': user.custom_data['uid'], 'cid': challenge['cid'] }
    existing_solution = mongodb.submissions.find_one(query_existing_solution)

    if existing_solution:
        # For an existing solution, we update the current siid and add the last one to the history
        # Additionally, we increase the RD value since a new submission might change the rating

        updated_previous_submissions = existing_solution['previous_submissions']
        updated_previous_submissions.append({'siid': existing_solution['siid']})

        update_document = { '$set': { 'siid': siid,
                                      'previous_submissions': updated_previous_submissions,
                                      'RD': max(160, existing_solution['RD']) } }
        
        mongodb.submissions.update(query_existing_solution, update_document)

    else:
        # For new solutions, we just add the document blueprint

        document = { 'siid': siid,
                     'cid': challenge['cid'],
                     'uid': user.custom_data['uid'],
                     'sid': str(uuid4()),
                     'rating': 1500,
                     'RD': 300.0,
                     'previous_submissions': [] }
        
        mongodb.submissions.insert(document)
    

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
        raise "Couldn't find a challenge with cid " + match['cid']
    
    users = list( mongodb.users.find({ 'uid': { '$in': [ user['uid'] for user in match['users'] ] } }) )

    time_delta = datetime.datetime.utcnow() - match['datetime']
    match['time_since'] = time_since_from_seconds( time_delta.total_seconds() )      
    match['challenge_name'] = challenge['name']
    match['cid'] = challenge['cid']
    match['visualizer'] = challenge['visualizer']
    match['usernames'] = [ user['username'] for user in users if user['uid'] in [match_user['uid'] for match_user in match['users']] ]
    match['users'] = users

    custom_title = ' vs '.join(match['usernames']) + ' on ' + match['challenge_name']

    return render_template('match.html', match = match, custom_title = custom_title)




@app.route('/matches')
def matches():
    """
    Page to display the 10 most recent matches.
    """
    matches = latest_matches(limit = 10)
    return render_template('matches.html', matches = matches)




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
        app.run(host='0.0.0.0', port=8080)
else:
    print "Try 'foreman start'"
