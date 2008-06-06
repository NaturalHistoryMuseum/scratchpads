#!/usr/bin/python

# Copyright (C) 2003 by BiRC -- Bioinformatics Research Center
#                               University of Aarhus, Denmark
#                               Contact: <mailund@birc.dk>


from newick.tree import parse_tree

def draw_tree(tree, draw_alg = 'linear'):
    from drawtree.measure import Measure
    from drawtree.canvas  import PostScriptCanvas
    from drawtree.draw    import LinearDraw, BoxDraw

    mes = Measure()
    tree.dfs_traverse(mes)

    canvas = PostScriptCanvas(tree.length, tree.leaves, mes.longest_label)
    if draw_alg == 'linear':
        draw = LinearDraw(canvas)
    elif draw_alg == 'box':
        draw = BoxDraw(canvas)

    canvas.begin()
    tree.dfs_traverse(draw)
    canvas.end()


if __name__ == '__main__':
    from sys    import argv,stdin
    from getopt import getopt

    opts, args = getopt(argv[1:], 'lb', ['linear', 'box'])

    draw_alg = 'linear'
    for (o,v) in opts:
        if o == '-l' or o == '--linear':
            draw_alg = 'linear'
        if o == '-b' or o == '--box':
            draw_alg = 'box'

    draw_tree(parse_tree(stdin.read()), draw_alg)
