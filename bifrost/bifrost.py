"""
    bifrost.py
    ~~~~~~

    Bifrost, or sometimes Bilrost or Bivrost, is a burning rainbow bridge
    that reaches between Midgard (the world) and Asgard, the realm of the gods.
"""

import markdown
import os
from os import environ
from uuid import uuid4

from flask import (
    Flask,
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
from wtforms.validators import DataRequired




##### Forms
class ChallengeDescriptionForm(Form):
    name = TextField('Challenge name', validators=[DataRequired()])
    pagedown = PageDownField('Challenge description', validators=[DataRequired()])




##### Initialization
app = Flask(__name__)

app.config['SECRET_KEY'] = environ.get('STORMPATH_SECRET_KEY')
app.config['STORMPATH_API_KEY_ID'] = environ.get('STORMPATH_API_KEY_ID')
app.config['STORMPATH_API_KEY_SECRET'] = environ.get('STORMPATH_API_KEY_SECRET')
app.config['STORMPATH_APPLICATION'] = environ.get('STORMPATH_APPLICATION')
app.config['MONGOLAB_URI'] = environ.get('MONGOLAB_URI')

app.jinja_env.globals.update(is_active_user_in=is_active_user_in)

def markdown_to_html(content):
    return Markup(markdown.markdown(content))

app.jinja_env.globals.update(markdown_to_html=markdown_to_html)


stormpath_manager = StormpathManager(app)
stormpath_manager.login_view = '.login'


mongo_client = MongoClient(app.config['MONGOLAB_URI'])
mongodb = mongo_client['mjollnir-db']
challenges_collection = mongodb.challenges


pagedown = PageDown(app)


##### Website
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
        # If something fails, we'll display a user-friendly error message.
        return render_template('register.html', error=err.message)


    return redirect(url_for('.login', x='verifymail'))




@app.route('/login', methods=['GET', 'POST'])
def login():
    """
    This view logs in a user given an email address and password.


    This works by querying Stormpath with the user's credentials, and either
    getting back the User object itself, or an exception (in which case well
    tell the user their credentials are invalid).


    If the user is valid, we'll log them in, and store their session for later.
    """
    if request.method == 'GET':
        return render_template('login.html')


    try:
        _user = User.from_login(
            request.form.get('email'),
            request.form.get('password'),
        )
    except StormpathError, err:
        return render_template('login.html', error=err.message)


    login_user(_user, remember=True)
    return redirect(request.args.get('next') or url_for('dashboard'))




@app.route('/dashboard', methods=['GET', 'POST'])
@login_required
def dashboard():
    """
    This view renders a simple dashboard page for logged in users.


    Users can see their personal information on this page, as well as store
    additional data to their account (if they so choose).
    """
    if request.method == 'POST':
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
    # This is potentially unsafe since there is no sanitizing on the markdown submitted to the db
    # Won't be a problem while admins are the only ones with access to submit challenges though
    #
    form = ChallengeDescriptionForm(csrf_enabled = False)

    if not form.pagedown.data:
        form.pagedown.data = '#Hey!\nEnter the <i>challenge description</i> using **Markdown**!'

    if request.method == 'GET':
        return render_template('newchallenge.html', form = form)

    if form.validate_on_submit():
        challenge_name = form.name.data
        challenge_description = form.pagedown.data
        challenge_id = uuid4()

        document = { 'cid': str(challenge_id),
                     'name': challenge_name,
                     'description': challenge_description,
                     'dev_only': True }

        try:
            challenges_collection.insert(document)
        # Might want to remove this if we give page access to non admins
        except PyMongoError as err:
            return render_template('newchallenge.html', form = form, error = err.message)

        return redirect(url_for('.challenge', cid = challenge_id))

    else:
        return render_template('newchallenge.html', form = form, error = "Please enter all the required information.")




@app.route('/challenge')
def challenge():
    """
    Page to display a challenge.
    """
    challenge_id = request.args.get('cid')

    if not challenge_id:
        return redirect(url_for('.index'))

    challenge = challenges_collection.find_one({"cid": challenge_id})

    if challenge and ( is_active_user_in('Dev') or not challenge['dev_only'] ):
        return render_template('challenge.html', challenge=challenge)
    else:
        return render_template('challenge.html', error = "No problem found for id " + challenge_id)




if __name__ == "__main__":
    if os.name == 'nt':
        app.run(host = '0.0.0.0', port = 80, debug = True)
