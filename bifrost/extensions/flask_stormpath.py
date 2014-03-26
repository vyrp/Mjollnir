"""
    extensions.flask_stormpath.py
    ~~~~~~

    A set of extensions for flask-stormpath.
"""

from functools import wraps
from flask import current_app
from flask.ext.login import current_user

def groups_allowed(groups=None):
    """
    Only allows access to the decorated view if the user belongs to any group in the
    set passed.
    """
    def decorator(func):

        @wraps(func)
        def decorated_view(*args, **kwargs):
            if current_app.login_manager._login_disabled:
                return func(*args, **kwargs)
            elif not current_user.is_authenticated():
                return current_app.login_manager.unauthorized()

            user_groups = [group.name for group in current_user.groups]
            for allowed_group in groups:
                if allowed_group in user_groups:
                    return func(*args, **kwargs)

            return current_app.login_manager.unauthorized()

        return decorated_view

    return decorator

def is_active_user_in(group):
    """
    Verifies if the current user is in a specific group.
    """
    return group in [group.name for group in current_user.groups]
