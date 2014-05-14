"""
    bifrost.py
    ~~~~~~

    Bifrost, or sometimes Bilrost or Bivrost, is a burning rainbow bridge
    that reaches between Midgard (the world) and Asgard, the realm of the gods.
"""

import boto
import markdown
import os
import traceback
from os import environ
from uuid import uuid4

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

from pymongo import MongoClient
from pymongo.errors import PyMongoError

from stormpath.error import Error as StormpathError

from werkzeug.utils import secure_filename

from wtforms.fields import TextField
from wtforms.fields import BooleanField
from wtforms.validators import DataRequired




##### Classes
class ChallengeDescriptionForm(Form):
    """
    WTForm to submit a challenge. WTF was chosen here so we could use Flask-PageDown.
    """
    name = TextField('Challenge name', validators=[DataRequired()])
    dev_only = BooleanField('Dev only')
    pagedown = PageDownField('Challenge description', validators=[DataRequired()])




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
app.jinja_env.globals.update(is_active_user_in=is_active_user_in)
app.jinja_env.globals.update(len=len)
app.jinja_env.globals.update(markdown_to_html=markdown_to_html)

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
    """Basic home page."""
    return render_template('index.html')




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
            mongodb.users.insert({ 
                'uid': str(uuid4()),
                'username': _user.username,
                'email': _user.email
            })

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

        submission['name'] = challenge['name']
        submission['RD'] = round(submission['RD'], 2)
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




@app.route('/newchallenge', methods=['GET', 'POST'])
@groups_allowed(['Dev'])
def newchallenge():
    """
    Allows a user in the "Dev" admin group to submit a new challenge.
    """
    form = ChallengeDescriptionForm(csrf_enabled = False)

    if not form.pagedown.data:
        form.pagedown.data = '#Hey!\nEnter the <i>challenge description</i> using **Markdown**!'

    if request.method == 'GET':
        return render_template('newchallenge.html', form = form)

    if form.validate_on_submit():
        # This is potentially unsafe since there is no sanitizing on the markdown submitted to the db
        # Won't be a problem while admins are the only ones with access to submit challenges though
        #
        challenge_id = uuid4()
        document = { 'cid': str(challenge_id),
                     'name': form.name.data,
                     'description': form.pagedown.data,
                     'dev_only': True }

        mongodb.challenges.insert(document)
        return redirect(url_for('.challenge_by_name', challenge_name = form.name.data))

    else:
        # TODO: unify editchallenge.html and newchallenge.html
        return render_template('newchallenge.html', form = form, error = "Please enter all the required information."), 400




@app.route('/editchallenge', methods=['GET', 'POST'])
@groups_allowed(['Dev'])
def editchallenge():
    """
    Allows a user in the "Dev" admin group to edit an existing challenge.
    """
    challenge_id = request.args.get('cid')

    if not challenge_id:
        abort(404)

    challenge = mongodb.challenges.find_one({ 'cid': challenge_id })

    if not challenge:
        abort(404)

    form = ChallengeDescriptionForm(csrf_enabled = False)

    if request.method == 'GET':
        form.name.data = challenge.get('name')
        form.dev_only.data = challenge.get('dev_only')
        form.pagedown.data = challenge.get('description')
        return render_template('editchallenge.html', form = form)

    if form.validate_on_submit():
        document = { 'cid': challenge_id,
                     'name': form.name.data,
                     'description': form.pagedown.data,
                     'dev_only': form.dev_only.data }

        mongodb.challenges.update({ 'cid': challenge_id }, document)
        return redirect(url_for('.challenge_by_name', challenge_name = form.name.data))

    else:
        # TODO: unify editchallenge.html and newchallenge.html
        return render_template('editchallenge.html', form = form, error = "Please enter all the required information."), 400




@app.route('/challenge/<challenge_name>')
def challenge_by_name(challenge_name):
    """
    Page to display a challenge given a problem name.
    """
    challenge = mongodb.challenges.find_one({"name": challenge_name})

    if not challenge or ( not is_active_user_in('Dev') and challenge['dev_only'] ):
        abort(404)

    submissions = mongodb.submissions.find({ 'cid': challenge['cid'] }).sort([ ('rating', -1) ])
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


    return render_template('challenge.html', challenge = challenge, challenge_solutions = challenge_solutions, custom_title = challenge_name)




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

    user_in_db = mongodb.users.find_one({ 'username': user.username })
    
    if not user_in_db:
        raise "Current user not found in the database. Try logging out and in again."

    file = request.files['sourcefile']

    if not file:
        return render_template('submitsolution.html', challenge = challenge, error = "Please select a Source File"), 400

    if not allowed_sourcefile(file.filename):
        return render_template('submitsolution.html', challenge = challenge, error = "Invalid Source File (did you use an invalid extension?)"), 403

    
    # Source Instance ID
    siid = str(uuid4())

    # Upload the source file to the 'mjollnir-solutions' S3 bucket using the siid as the key
    filename = secure_filename(file.filename)
    solutions_bucket = s3.get_bucket('mjollnir-solutions')
    key = solutions_bucket.new_key(siid)
    key.set_contents_from_file(file, headers=None, replace=True, cb=None, num_cb=10, policy=None, md5=None) 

    # Update/Create a database entry for this submission
    query_existing_solution = { 'uid': user_in_db['uid'], 'cid': challenge['cid'] }
    existing_solution = mongodb.submissions.find_one(query_existing_solution)

    if existing_solution:
        # For an existing solution, we update the current siid and add the last one to the history
        # Additionally, we increase the RD value since a new submission might change the rating

        updated_previous_submissions = existing_solution['previous_submissions']
        updated_previous_submissions.append({'siid': existing_solution['siid'], 'language': existing_solution['language']})

        update_document = { '$set': { 'siid': siid,
                                      'language': filename.rsplit('.', 1)[1],
                                      'previous_submissions': updated_previous_submissions,
                                      'RD': max(160, existing_solution['RD']) } }
        
        mongodb.submissions.update(query_existing_solution, update_document)

    else:
        # For new solutions, we just add the document blueprint

        document = { 'siid': siid,
                     'language': filename.rsplit('.', 1)[1],
                     'cid': challenge['cid'],
                     'uid': user_in_db['uid'],
                     'sid': str(uuid4()),
                     'rating': 1500,
                     'RD': 320.0,
                     'previous_submissions': [] }
        
        mongodb.submissions.insert(document)
    

    return redirect(url_for('dashboard'))

ALLOWED_EXTENSIONS = set(['cs', 'cpp', 'py'])
def allowed_sourcefile(filename):
    return '.' in filename and filename.rsplit('.', 1)[1] in ALLOWED_EXTENSIONS




@app.route('/teapot')
def teapot():
    """
    Allows a user to verify if this server is a teapot.
    """
    abort(418)




# On unix systems the project should be executed using Gunicorn and Foreman.
# Since Gunicorn doesn't run in windows yet, we let Flask itself handle the requests on nt systems.
if os.name == 'nt':
    if __name__ == "__main__":
        app.run(host='0.0.0.0', port=8080)
else:
    print "Try 'foreman start'"
