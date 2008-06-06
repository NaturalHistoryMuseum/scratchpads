import tokens
import re

class ParseError:
    def __init__(self,err):
        self.err = err

    def __repr__(self):
        return "ParseError: "+self.err

### --- LEXER -----------------------------------------------------------
patterns = [
    (tokens.Number, 	re.compile(r'\s*(-?\d*\.?\d+(e-?\d+)?)\s*')),
    (tokens.ID, 	re.compile(r"\s*(('[^']+')|([\w/-]|_)+)\s*")),
    (tokens.Colon, 	re.compile(r'\s*(:)\s*')),
    (tokens.SemiColon, 	re.compile(r'\s*(;)\s*')),
    (tokens.Comma, 	re.compile(r'\s*(,)\s*')),
    (tokens.LParen, 	re.compile(r'\s*(\()\s*')),
    (tokens.RParen, 	re.compile(r'\s*(\))\s*'))
    ]


class Lexer:
    def __init__(self, input):
        self.input = input
        self.next_token = None

    def peek_next_token(self):
        ''' return the next token in the input, without deleting it
        from the input stream. '''
        if self.next_token:
            return self.next_token
        else:
            for (cons, p) in patterns:
                m = re.match(p,self.input)
                if m:
                    self.next_token = cons(self.input[m.start():m.end()])
                    self.input = self.input[m.end():]
                    return self.next_token
            # no match, either end of string or lex-error
            if self.input != '':
                raise ParseError("Lex Error, unknown token at "
                                 +self.input[:10]+"...")
            else:
                return None

    def get_next_token(self):
        ''' return (and delete) the next token from the input
        stream. '''
        token = self.peek_next_token()
        self.next_token = None
        return token

    def read_token(self,token_class):
        ''' Read a token of the specified class, or raise an exception
        if the next token is not of the given class. '''
        token = self.get_next_token()
        if token.__class__ != token_class:
            raise ParseError("expected "+str(token_class)+
                             " but received "+str(token.__class__)+
                             " at "+self.input[:10]+"...")
        else:
            return token

    def peek_token(self,token_class):
        ''' checks whether the next token is of the specified class. '''
        token = self.peek_next_token()
        return token.__class__ == token_class


### --- PARSER ----------------------------------------------------------

class AbstractHandler:
    def __init__(self):
        pass

    def new_tree_begin(self):
        pass
    def new_tree_end(self):
        pass

    def new_edge(self, bootstrap, length):
        pass
    def new_leaf(self, name):
        pass

class Parser:
    def __init__(self, lexer, handler):
        self.lexer = lexer
        self.handler = handler

    def parse(self):
        if self.lexer.peek_token(tokens.LParen):
            return self.parse_node()
        else:
            return self.parse_leaf()

    def parse_node(self):
        ''' Parse node on the form ( <edge list> ) '''
        self.lexer.read_token(tokens.LParen)
        self.handler.new_tree_begin()
        self.parse_edge_list()
        self.handler.new_tree_end()
        self.lexer.read_token(tokens.RParen)

    def parse_leaf(self):
        ''' Parse a node on the form "id" '''
        if self.lexer.peek_token(tokens.Comma) or \
               self.lexer.peek_token(tokens.RParen):
            # blank name
            self.handler.new_leaf("")
            return
            
        id = self.lexer.read_token(tokens.ID).get_name()
        if id == '_':
            # blank name
            self.handler.new_leaf('')
        else:
            self.handler.new_leaf(id)

        
    def parse_edge_list(self):
        ''' parse a comma-separated list of edges. '''
        while 1:
            self.parse_edge()

            if self.lexer.peek_token(tokens.Comma):
                self.lexer.read_token(tokens.Comma)
            else:
                break
               

    def parse_edge(self):
        ''' Parse a single edge, either leaf [bootstrap] [: branch-length]
        or tree [bootstrap] [: branch-length]. '''
        if self.lexer.peek_token(tokens.LParen):
            self.parse_node()
        else:
            self.parse_leaf()

        if self.lexer.peek_token(tokens.Number):
            bootstrap = self.lexer.read_token(tokens.Number).get_number()
        else:
            bootstrap = None

        if self.lexer.peek_token(tokens.Colon):
            self.lexer.read_token(tokens.Colon)
            length = self.lexer.read_token(tokens.Number).get_number()
        else:
            length = None

        self.handler.new_edge(bootstrap,length)



if __name__ == '__main__':
    print "Testing lexer...",
    lexer = Lexer("()'foo' bar :0.00,;")
    lexer.read_token(tokens.LParen)
    lexer.read_token(tokens.RParen)
    id = lexer.read_token(tokens.ID)
    if id.get_name() != 'foo':
        raise "Unexpected name!"
    id = lexer.read_token(tokens.ID)
    if id.get_name() != 'bar':
        raise "Unexpected name!"
    lexer.read_token(tokens.Colon)
    n = lexer.read_token(tokens.Number)
    if n.get_number() != 0.00:
        raise "Unexpected number!"
    lexer.read_token(tokens.Comma)
    lexer.read_token(tokens.SemiColon)
    print "Done"

    print "Testing parse...",
    import tree
    lexer = Lexer("(('foo' : 0.1, 'bar' : 1.0) : 2, baz)")
    handler = tree.TreeBuilder()
    parser = Parser(lexer,handler)
    parser.parse()
    t = handler.get_tree()

    if len(t.get_edges()) != 2:
        raise "Unexpected number of edges"
    [(t1,b1,l1), (t2,b2,l2)] = t.get_edges()
    if len(t1.get_edges()) != 2:
        raise "Unexpected number of edges"
    if l1 != 2.0:
        raise "Unexpected edge length"

    if t2.__class__ != tree.Leaf:
        raise "Leaf expected"
    if l2 != None:
        raise "Unexpected edge length"


    t = tree.parse_tree("(('foo' : 0.1, 'bar' : 1.0) : 2, baz)")

    if len(t.get_edges()) != 2:
        raise "Unexpected number of edges"
    [(t1,b1,l1), (t2,b2,l2)] = t.get_edges()
    if len(t1.get_edges()) != 2:
        raise "Unexpected number of edges"
    if l1 != 2.0:
        raise "Unexpected edge length"

    if t2.__class__ != tree.Leaf:
        raise "Leaf expected"
    if l2 != None:
        raise "Unexpected edge length"



    class BranchLengthSum(AbstractHandler):
        def __init__(self):
            self.sum = 0.0

        def new_edge(self,bootstrap,length):
            if length:
                self.sum += length

        def get_sum(self):
            return self.sum

    lexer = Lexer("(('foo' : 0.1, 'bar' : 1.0) : 2, baz)")
    handler = BranchLengthSum()
    parser = Parser(lexer,handler)
    parser.parse()
    sum = handler.get_sum()

    if sum != 3.1:
        raise "Unexpected sum"

    from tree import parse_tree
    tree = parse_tree("(B,(A,C,E),D);")
    print tree
    tree = parse_tree("(,(,,),);")
    print tree
    tree = parse_tree("(_,(_,_,_),_);")
    print tree

    tree = parse_tree("A;")
    print tree

    print "Done"

    
