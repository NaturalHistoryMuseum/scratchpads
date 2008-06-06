class Tree:
    '''
    Python representation of a tree (or rather an inner node in a tree).
    '''

    def __init__(self):
        self.edges = []

    def add_edge(self,e):
        '''
        add_edge(e) -- add edge to sub-tree.

        Insert an edge to a new sub-tree.  The edge should be on the
        form: (st,bo,le), where st is the sub-tree, bo is the
        bootstrap value of the edge, and le is the length of the tree.
        '''
        self.edges.append(e)

    def get_edges(self):
        '''
        get_edges() -- return the list of edges to sub-trees.
        '''
        return self.edges


    def dfs_traverse(self,visitor):
        '''
        dfs_traverse(visitor) -- do a depth first traversal.

        Part of the Visitor Pattern; performs a depth first traversal,
        calling methods in visitor along the way.
        '''
        visitor.pre_visit_tree(self)
        for (n,b,l) in self.edges:
            visitor.pre_visit_edge(self,b,l,n)
            n.dfs_traverse(visitor)
            visitor.post_visit_edge(self,b,l,n)
        visitor.post_visit_tree(self)


    def __repr__(self):
        tree_str = '('
        sep = ''
        for (n,b,l) in self.edges:
            tree_str += sep+str(n)
            if b:
                tree_str += str(b) + ' '
            if l:
                tree_str += ' : ' + str(l)
            sep = ', '
        return tree_str+')'

class Leaf:
    '''
    Python representation of a leaf in a tree.
    '''

    def __init__(self, id):
        '''
        Leaf(id) -- construct leaf with label id.
        '''
        self.id = id


    def dfs_traverse(self,visitor):
        '''
        dfs_traverse(visitor) -- do a depth first traversal.

        Part of the Visitor Pattern; calls the visit_leaf callback in visitor.
        '''
        visitor.visit_leaf(self)


    def __repr__(self):
        return "'"+self.id+"'"


class TreeVisitor:
    '''
    Part of the Visitor Pattern.
    '''

    def __init__(self):
        pass

    def pre_visit_tree(self,t):
        '''
        pre_visit_tree(t) -- callback called before exploring (sub-)tree t.
        '''
        pass
    def post_visit_tree(self,t):
        '''
        post_visit_tree(t) -- callback called after exploring (sub-)tree t.
        '''
        pass

    def pre_visit_edge(self,src,bootstrap,length,dst):
        '''
        pre_visit_edge(src, bo,le, dst)
        	-- callback called before exploring an edge.

        Here src is the source node and dst is the destination node,
        bo is the bootstrap support of the edge and le is the length
        of the edge.
        '''
        pass
    def post_visit_edge(self,src,bootstrap,length,dst):
        '''
        post_visit_edge(src, bo,le, dst)
        	-- callback called before exploring an edge.

        Here src is the source node and dst is the destination node,
        bo is the bootstrap support of the edge and le is the length
        of the edge.
        '''
        pass

    def visit_leaf(self,l):
        '''
        visit_leaf(l) -- callback called when exploring leaf l.
        '''
        pass


import newick
class TreeBuilder(newick.AbstractHandler):
    def __init__(self):
        self.stack = []
        self.root = None

    def new_tree_begin(self):
        t = Tree()
        if len(self.stack) == 0:
            self.root = t
        self.stack.append(t)

    def new_edge(self,bootstrap,length):
        n = self.stack.pop()
        self.stack[-1].add_edge((n,bootstrap,length))

    def new_leaf(self,l):
        if len(self.stack) == 0:        # special case of singleton tree
            self.root = l
        self.stack.append(Leaf(l))

    def get_tree(self):
        return self.root

def parse_tree(input):
    builder = TreeBuilder()
    lexer = newick.Lexer(input)
    newick.Parser(lexer,builder).parse()
    return builder.get_tree()

    
