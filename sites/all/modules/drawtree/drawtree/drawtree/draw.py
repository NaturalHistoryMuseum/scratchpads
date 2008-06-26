# Copyright (C) 2003 by BiRC -- Bioinformatics Research Center
#                               University of Aarhus, Denmark
#                               Contact: <mailund@birc.dk>

'''

This module contains the tree drawing algorithms.

'''

from newick.tree import TreeVisitor

class AbstractDraw(TreeVisitor):
    ''' Interface for drawing algorithms. '''

    def __init__(self, canvas):
        '''
        AbstractDraw(canvas) -- initialise algorithm to draw on canvas.
        '''
        TreeVisitor.__init__(self)
        self.canvas = canvas

        self.current_heigth = 0
        self.current_depth = 0

    def visit_leaf(self,l):
        '''
        pre_visit_leaf(l) -- callback called before exploring leaf l.

        Sets coordinates for leaf and performs book-keeping for heigth.
        '''
        l.x = self.current_depth
        l.y = self.current_heigth
        self.current_heigth += 1


    def pre_visit_edge(self, src, bs, le, dst):
        '''
        pre_visit_edge(src, bo,le, dst)
        	-- callback called before exploring an edge.

        Performs book-keeping for depth.
        '''
        if le:
            self.current_depth += le
        else:
            self.current_depth += 0

    def post_visit_edge(self, src, bs, le, dst):
        '''
        post_visit_edge(src, bo,le, dst)
        	-- callback called before exploring an edge.

        Performs book-keeping for depth.
        '''
        if le:
            self.current_depth -= le
        else:
            self.current_depth -= 0

    def pre_visit_tree(self,t):
        '''
        pre_visit_tree(t) -- callback called before exploring (sub-)tree t.

        Sets coordinates for tree.
        '''
        t.x = self.current_depth
        t.y = self.current_heigth + 0.5*(t.leaves-1)


class LinearDraw(AbstractDraw):
    '''
    Algorithm for drawing linear trees (or whatever you want to call them).
    '''

    def __init__(self, canvas):
        '''
        LinearDraw(canvas) -- initialise algorithm to draw on canvas.
        '''
        AbstractDraw.__init__(self,canvas)

    def visit_leaf(self,l):
        '''
        pre_visit_leaf(l) -- callback called before exploring leaf l.

        Draws leaf l.
        '''
        AbstractDraw.visit_leaf(self,l)

        self.canvas.draw_node(l.x,l.y)
        self.canvas.draw_label(l.x,l.y, l.id)

    def post_visit_edge(self, src, bs, le, dst):
        '''
        post_visit_edge(src, bo,le, dst)
        	-- callback called before exploring an edge.

        Draws edge.
        '''
        AbstractDraw.post_visit_edge(self, src, bs, le, dst)

        self.canvas.draw_edge(src.x,src.y, dst.x,dst.y, bs)

    def pre_visit_tree(self,t):
        '''
        pre_visit_tree(t) -- callback called before exploring (sub-)tree t.

        Draws tree-node.
        '''
        AbstractDraw.pre_visit_tree(self, t)

        self.canvas.draw_node(t.x,t.y)

class BoxDraw(AbstractDraw):
    '''
    Algorithm for drawing "box" trees (or whatever you want to call them).
    '''

    def __init__(self, canvas):
        '''
        BoxDraw(canvas) -- initialise algorithm to draw on canvas.
        '''
        AbstractDraw.__init__(self,canvas)

    def visit_leaf(self,l):
        '''
        pre_visit_leaf(l) -- callback called before exploring leaf l.

        Draws leaf l.
        '''
        AbstractDraw.visit_leaf(self,l)

        self.canvas.draw_node(l.x,l.y)
        self.canvas.draw_label(l.x,l.y, l.id)

    def post_visit_edge(self, src, bs, le, dst):
        '''
        post_visit_edge(src, bo,le, dst)
        	-- callback called before exploring an edge.

        Draws edge.
        '''
        AbstractDraw.post_visit_edge(self, src, bs, le, dst)

        self.canvas.draw_edge(src.x,src.y, src.x,dst.y, bs)
        self.canvas.draw_edge(src.x,dst.y, dst.x,dst.y, bs)

    def pre_visit_tree(self,t):
        '''
        pre_visit_tree(t) -- callback called before exploring (sub-)tree t.

        Draws tree-node.
        '''
        AbstractDraw.pre_visit_tree(self, t)

        self.canvas.draw_node(t.x,t.y)
