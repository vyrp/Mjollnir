"""
    extensions.string_building.py
    ~~~~~~

    A set of extensions for building strings
"""

def time_since_from_seconds(seconds):
    days = int( seconds // 86400 ) 
    hours = int( (seconds % 86400) // 3600 )
    minutes = int( (seconds % 3600) // 60 )

    time_since = ''
    if days > 7:
        time_since = 'a long time ago'
    else:
        if days > 0:
            time_since += str(days) + 'd '
        if days > 0 or hours > 0:
            time_since += str(hours) + 'h '
        time_since += str(minutes) + 'm ago'

    return time_since

def timestamp_as_str(date):
    return date.strftime("%b %d '%y at %H:%M UTC")
