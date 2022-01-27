import datetime


class ClientEntry(object):
    def __init__(self, id, user, pub_key):
        self.ID = id
        self.UserName = user
        self.PubKey = pub_key
        self.LastSeen = datetime.datetime.now()
