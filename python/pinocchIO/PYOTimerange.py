from datetime import timedelta, datetime


def FromTimeset(timeset, origin=datetime(year=1981, month=4, day=17)):
    """
    Create PYOTimerange based on 'timeset' entry (internal 3-dimensional pinocchIO array).
        - timeset[2] is number of units per seconds
        - timeset[0] is time in number of units
        - timeset[1] is duration in number of units
        - if origin is provided, time range is shifted with its value
          Default origin is April 17th, 1981.
    """
    time = timeset[0]
    duration = timeset[1]
    scale = timeset[2]
    return PYOTimerange(origin+timedelta(seconds=1.0*time/scale), timedelta(seconds=1.0*duration/scale))


def WithMiddleAndDuration(middle, duration):   
    """
    Create PYOTimerange from its middle and duration
        -  middle as datetime.datetime
        - duration as datetime.timedelta
    """ 
    return PYOTimerange(middle - timedelta(seconds=.5*duration.total_seconds()), duration)


def FromStartToStop(start, stop):
    """
    Create PYOTimerange between start and stop time
        - start as datetime.datetime
        - stop as datetime.datetime
    """
    return PYOTimerange(start, stop-start)


class PYOTimerange(object):
    def __init__(self, start, duration):
        super(PYOTimerange, self).__init__()
        self.start = start
        self.duration = duration
    
    
    def getStart(self):
        """
        Get start time as datetime.datetime
        """
        return self.start
    
    
    def getStop(self):
        """
        Get stop time as datetime.datetime
        """
        return self.start + self.duration
    
    
    def getDuration(self):
        """
        Get duration as datetime.timedelta
        """
        return self.duration
    
    
    def getMiddle(self):
        """
        Get middle time as datetime.datetime
        """
        return self.start + timedelta(seconds=.5*self.duration.total_seconds())
    
    
    def equals(self, timerange):
        """
        Returns True if identical to provided time range
            and False otherwise
        """
        return cmp(self.getStart(), timerange.getStart()) == 0 and cmp(self.getStop(), timerange.getStop()) == 0
    
    
    def contains(self, timestamp, strict=False):
        """
        Returns True if timestamp (as datetime.datetime) happens during time range
            and False otherwise
        """
        if strict:
            return cmp(self.getStart(), timestamp) < 0 and cmp(self.getStop(), timestamp) > 0
        else:
            return cmp(self.getStart(), timestamp) <= 0 and cmp(self.getStop(), timestamp) >= 0
    
    
    def intersection(self, timerange):
        """
        Returns intersection with other time range (as PYOTimerange) if they do intersect
            and None otherwise
        """
        
        start = max(self.getStart(), timerange.getStart())
        stop  = min(self.getStop(),  timerange.getStop())
        duration = stop - start
        if duration.total_seconds() > 0:
            return PYOTimerange(start, duration)
        else:
            return None
    
    
    def intersects(self, timerange):
        """
        Returns True if intersection with other time range is not empty
            and False otherwise
        """
        return self.intersection(timerange) != None
    
    
    def includes(self, timerange, strict=False):
        """
        Returns True if other time range is fully included in time range
            and False otherwise
        """
        if strict:
            return cmp(self.getStart(), timerange.getStart()) < 0 and cmp(self.getStop(), timerange.getStop()) > 0
        else:
            return cmp(self.getStart(), timerange.getStart()) <= 0 and cmp(self.getStop(), timerange.getStop()) >= 0
    
    
    def included(self, timerange, strict=False):
        """
        Returns True if time range is included in other time range
        """
        return timerange.includes(self, strict=strict)
    
    
    def compare(self, timerange):
        """
        Compare relative position of time range [******] with other time range [------]
        
                      [******]
                      |      | [------]    => -6
                      |      [------]      => -5
                      |  [---|--]          => -4
                      | [----]             => -3
                      | [--] |             => -2
                      [------|--]          => -1
                      [------]             =>  0
                      [----] |             =>  1
                   [--|------|--]          =>  2
                   [--|------]             =>  3
                  [---|--]   |             =>  4
                [-----]      |             =>  5
              [-----] |      |             =>  6
                      [******]
        """
        # =============================================== #
        #                  [***...                        #
        #               [---...                           #
        # =============================================== #
        if cmp(timerange.getStart(), self.getStart()) < 0:
            return -timerange.compare(self)
        
        # =============================================== #
        #                  [***...                        #
        #                  [---...                        #
        # =============================================== #
        if cmp(timerange.getStart(), self.getStart()) == 0:
            
            # =============================================== #
            #                  [******]                       #
            #                  [------]                       #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) == 0:
                return 0
            
            # =============================================== #
            #                  [******]                       #
            #                  [---]                          #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) < 0:
                return -1
            
            # =============================================== #
            #                  [******]                       #
            #                  [---------]                    #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) > 0:
                return 1
            
        # =============================================== #
        #                  [***...                        #
        #                     [---...                     #
        # =============================================== #
        if cmp(timerange.getStart(), self.getStart()) > 0:
            
            # =============================================== #
            #             ...***]                             #
            #                   |  [---...                    #
            # =============================================== #
            if cmp(timerange.getStart(), self.getStop()) > 0:
                return -6
            
            # =============================================== #
            #             ...***]                             #
            #                   [---...                       #
            # =============================================== #
            if cmp(timerange.getStart(), self.getStop()) == 0:
                return -5
            
            # =============================================== #
            #             [******]                            #
            #                 [--|---]                        #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) > 0:
                return -4
            
            # =============================================== #
            #             [******]                            #
            #               [----]                            #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) == 0:
                return -3
            
            # =============================================== #
            #             [******]                            #
            #               [--]                              #
            # =============================================== #
            if cmp(timerange.getStop(), self.getStop()) < 0:
                return -2
    
    
    def __repr__(self):
        return "<PYOTimerange [%s --> %s]>" % (self.getStart(), self.getStop())
    
    
    def __str__(self):
        return "[%s --> %s]" % (self.getStart(), self.getStop())
    

