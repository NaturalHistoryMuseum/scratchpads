# Copyright (C) 2003 by BiRC -- Bioinformatics Research Center
#                               University of Aarhus, Denmark
#                               Contact: <mailund@birc.dk>

'''

This module contains wrappers for different output formats.

'''

class AbstractCanvas:
    ''' Interface for output format wrappers. '''

    def __init__(self):
        pass

    def begin(self):
        '''
        begin()

        Callback called prior to the actual tree drawing.  Usefull
        for writing preambles.
        '''
        pass

    def draw_node(self, x, y):
        '''
        draw_node(x,y) -- draw node at point (x,y).

        Callback called for each tree node.  Can be used to, for
        instance, print a "dot".
        '''
        pass

    def draw_label(self, x,y, l):
        '''
        draw_label(x,y, l) -- print text label l at point (x,y).

        Callback called each time text should be written.
        '''
        pass

    def draw_edge(self, x1,y1, x2,y2):
        '''
        draw_edge(self, x1,y1, x2,y2, bootstrap)
        	-- draw line, with bootstrap support, from (x1,y1) to (x2,y2)

        Callback called each time a line should be drawn.  The
        bootstrap value can be used to modify the appearance of the
        line to better visualise the bootstrap value.
        '''
        pass

    def end(self):
        '''
        end()

        Callback called when the entire tree has been drawn.  Used for
        cleaning up.
        '''
        pass


# FIXME: the way I handle labels here is a hack, but I don't know how
# to calculate the right bounding box... :(
class PostScriptCanvas(AbstractCanvas):
    def __init__(self, depth, leaves, label_length):
        AbstractCanvas.__init__(self)

        self.b_width = 400
        self.width = depth

        self.leaves = leaves

        self.label_length = 10*label_length # assuming chars in 10 pt
        self.font_size = 14
        self.font_sink = self.font_size/4

    def begin(self):
        '''
        begin()

        Callback called prior to the actual tree drawing.  Usefull
        for writing preambles.
        '''

        print '%!PS-Adobe EPSF-3.0'
        print '%%BoundingBox: -1 ', -(.5*self.font_size),
        print self.b_width, (self.leaves)*self.font_size-(.25*self.font_size)

        print '/Times-Roman findfont', self.font_size, 'scalefont setfont'

        print '/xunit {', (self.b_width-self.label_length)/self.width,
        print 'mul} bind def'
        print '/yunit {', self.font_size, 'mul} bind def'

        print 'gsave'

    def draw_node(self, x, y):
        '''
        draw_node(x,y) -- draw node at point (x,y).

        Callback called for each tree node.  Can be used to, for
        instance, print a "dot".
        '''
        pass

    def draw_label(self, x,y, l):
        '''
        draw_label(x,y, l) -- print text label l at point (x,y).

        Callback called each time text should be written.
        '''

        print 'newpath', x,'xunit', y,'yunit ', -self.font_sink, 'add moveto',
        print '('+l+') show'

    def draw_edge(self, x1,y1, x2,y2, bootstrap):
        '''
        draw_edge(self, x1,y1, x2,y2, bootstrap)
        	-- draw line, with bootstrap support, from (x1,y1) to (x2,y2)

        Callback called each time a line should be drawn.  The
        bootstrap value can be used to modify the appearance of the
        line to better visualise the bootstrap value.
        '''

        if bootstrap == None:
            bootstrap = 1

        elif 1 < bootstrap < 100:
            bootstrap = .01*bootstrap

        if bootstrap < 1:
            gray_tone = 1 - bootstrap
        else:
            gray_tone = 0
            
        print 'gsave'
        print gray_tone, 'setgray'
        print x1, 'xunit', y1, 'yunit', 'moveto', x2, 'xunit', y2, 'yunit',
        print 'lineto stroke'
        print 'grestore'


    def end(self):
        '''
        end()

        Callback called when the entire tree has been drawn.  Used for
        cleaning up.
        '''

        print 'grestore'
