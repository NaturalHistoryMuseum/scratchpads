# Copyright (C) 2003 by BiRC -- Bioinformatics Research Center
#                               University of Aarhus, Denmark
#                               Contact: <mailund@birc.dk>

'''

This module contains the visitors used to examine and measure
tree-dimensions prior to drawing the tree.

'''

from newick.tree import TreeVisitor

class Measure(TreeVisitor):
    def __init__(self):
        TreeVisitor.__init__(self)
        self.longest_label = 0
        
    def visit_leaf(self,l):
        l.leaves = 1
        l.length = 0

        self.longest_label = max(self.longest_label, len(l.id))

    def post_visit_tree(self,t):
        count = 0
        length = 0

        for (n,b,l) in t.get_edges():
            count += n.leaves
            if l:
                corrected_l = l
            else:
                corrected_l = 0

            length = max(length, n.length+l)

        t.leaves = count
        t.length = length

