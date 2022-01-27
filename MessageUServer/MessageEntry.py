class MessageEntry(object):
    def __init__(self, id, to_id, from_id, type, content):
        self.ID = id
        self.ToClient = to_id
        self.FromClient = from_id
        self.Type = type
        self.Content = content