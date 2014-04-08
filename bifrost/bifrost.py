"""
    bifrost.py
    ~~~~~~

    Bifrost, or sometimes Bilrost or Bivrost, is a burning rainbow bridge
    that reaches between Midgard (the world) and Asgard, the realm of the gods.
"""

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

from pymongo import MongoClient
from pymongo.errors import PyMongoError

from stormpath.error import Error as StormpathError

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

# Export handy functions to jinja
def markdown_to_html(content):
    return Markup(markdown.markdown(content))

app.jinja_env.globals.update(markdown_to_html=markdown_to_html)
app.jinja_env.globals.update(is_active_user_in=is_active_user_in)
app.jinja_env.globals.update(format_exc = traceback.format_exc)

# Stormpath
stormpath_manager = StormpathManager(app)
stormpath_manager.login_view = '.login'

# Mongodb
mongo_client = MongoClient(app.config['MONGOLAB_URI'])
mongodb = mongo_client['mjollnir-db']
challenges_collection = mongodb.challenges

# Pagedown
pagedown = PageDown(app)




##### Website

### Error Handlers
@app.errorhandler(404)
def page_not_found(error):
    """
    Handler for HTTP 404.
    """
    return render_template('error.html', description = '404: Not Found :(', error = error), 404




@app.errorhandler(Exception)
def exception_handler(error):
    """
    Handler for HTTP 500 on unexpected exceptions.
    """   
    return render_template('error.html', description = '500: Internal Server Error :(', error = error), 500




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
            'given_name': 'Xupa',
            'surname': 'Croata',
        })
        _user.__class__ = User
    except StormpathError, err:
        return render_template('register.html', error=err.message)

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
    except StormpathError, err:
        # TODO, some errors have wrong messages (e.g. 'incorrect password' upon unverified account)
        return render_template('login.html', error=err.message)

    login_user(_user, remember=True)
    return redirect(request.args.get('next') or url_for('dashboard'))




@app.route('/dashboard', methods=['GET', 'POST'])
@login_required
def dashboard():
    """
    Renders a simple dashboard page for logged in users.
    """
    if request.method == 'POST':
        # This is a sample on how to store user data in Stormpath. 
        # We might never do it, but let's keep it here for now anyways.
        if request.form.get('birthday'):
            user.custom_data['birthday'] = request.form.get('birthday')

        if request.form.get('color'):
            user.custom_data['color'] = request.form.get('color')

        user.save()

    return render_template('dashboard.html')




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

        challenges_collection.insert(document)
        return redirect(url_for('.challenge', cid = challenge_id))

    else:
        return render_template('newchallenge.html', form = form, error = "Please enter all the required information.")




@app.route('/editchallenge', methods=['GET', 'POST'])
@groups_allowed(['Dev'])
def editchallenge():
    """
    Allows a user in the "Dev" admin group to edit an existing challenge.
    """
    challenge_id = request.args.get('cid')

    if not challenge_id:
        abort(404)

    challenge = challenges_collection.find_one({ 'cid': challenge_id })

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

        challenges_collection.update({ 'cid': challenge_id }, document)
        return redirect(url_for('.challenge', cid = challenge_id))

    else:
        return render_template('editchallenge.html', form = form, error = "Please enter all the required information.")




@app.route('/challenge')
def challenge():
    """
    Page to display a challenge.
    """
    challenge_id = request.args.get('cid')

    if not challenge_id:
        abort(404)

    challenge = challenges_collection.find_one({"cid": challenge_id})

    if challenge and ( is_active_user_in('Dev') or not challenge['dev_only'] ):
        return render_template('challenge.html', challenge=challenge)
    else:
        abort(404)




# On unix systems the project should be executed using Gunicorn and Foreman.
# Since Gunicorn doesn't run in windows yet, we let Flask itself handle the requests on nt systems.
if os.name == 'nt':
    if __name__ == "__main__":
        app.run(host = '0.0.0.0', port = 80)
