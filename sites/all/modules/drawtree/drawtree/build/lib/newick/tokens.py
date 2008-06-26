
class Token:
    def __init__(self, str):
        self.str = str

    def __repr__(self):
        return 'T"'+self.str+'"'

class LParen(Token):
    pass

class RParen(Token):
    pass

class ID(Token):
    def __init__(self, id):
        id = id.strip()
        if id[0] == "'":
            id = id[1:-1]
        self.id = id

    def get_name(self):
        return self.id

    def __repr__(self):
        return 'ID"'+self.id+'"'

class Colon(Token):
    pass

class SemiColon(Token):
    pass

class Comma(Token):
    pass

class Number(Token):
    def __init__(self, number):
        self.number = float(number)

    def get_number(self):
        return self.number

    def __repr__(self):
        return 'NUMBER"'+str(self.number)+'"'
