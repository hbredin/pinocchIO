
from pinocchIO import PYODataset, PYOTimeline, PYOTimerange
import pinocchIO.utils.timeline
import pinocchIO.utils.aggregator.frequency
import numpy as np


def sub(dataset, extent):

    timeline = pinocchIO.utils.timeline.sub(dataset.getTimeline(), extent)
    if timeline.isEmpty():
        return PYODataset.Empty()
    
    trIDs  = dataset.getTimeline().indexOfTimerangesInPeriod(extent, strict=False)
    # note: trIDs cannot be empty -- otherwise timeline would have been empty
    #       and we would have already returned an empty dataset
    
    number = dataset.getNumber()[trIDs]
    data   = np.concatenate([dataset[i] for i in trIDs]) 
    
    return PYODataset.PYODataset(data, number, timeline)


def paste(datasets):
    
    # TODO => check all timelines are the same
    # TODO => check all datasets are the same length
    
    try:
        data = np.concatenate([dataset.data for d, dataset in enumerate(datasets)], axis=1)            
    except Exception, e:
        sys.stderr.write("Pasted datasets must have identical number of entries.")
        data = None
            
    return PYODataset.PYODataset(data, datasets[0].getNumber(), datasets[0].getTimeline())


def aggregate(dataset, timeline, aggregator=None, **aggregator_param):
    
    # nothing to aggregate if dataset and/or timeline are/is empty
    if dataset.isEmpty() or timeline.isEmpty():
        return PYODataset.Empty()
    
    # params will be passed to the aggregator function
    # it is meant to contain the user-provided parameters
    # and additional parameters (such as the current timerange)
    params = {}
    for p, param in enumerate(aggregator_param):
        params[param] = aggregator_param[param]
    
    nTimeranges = timeline.getNumberOfTimeranges()
    datas = [None] * nTimeranges
    number = np.zeros((nTimeranges, ), dtype=np.int32)
    for t, timerange in enumerate(timeline):
        
        subdataset = sub(dataset, timerange)
        
        # try and aggregate subdataset:
        # some aggregator may fail in case of empty dataset
        # some other may return something
        
        try:
            if aggregator == None:
                datas[t] = np.copy(subdataset.data)
            else:
                params['timerange'] = timerange
                datas[t] = aggregator(subdataset, **params)
            number[t] = datas[t].shape[0]            
        except Exception, e:
            raise e
            datas[t] = None
            number[t] = 0
    
    data = np.concatenate([d for d in datas if d != None], axis=0)
    
    return PYODataset.PYODataset(data, number, timeline)


def ascii(dataset, f, origin=PYOTimerange.DEFAULT_ORIGIN):
    
    timeline  = dataset.getTimeline()
    dimension = dataset.getDimension()
    
    for t, timerange in enumerate(timeline):
        
        data = dataset[t]        
        number = data.shape[0]
        
        xL = (timerange.getStart() - origin).total_seconds()
        xR = (timerange.getStop()  - origin).total_seconds()
        
        f.write('%g %g' % (xL, xR))
        for n in range(number):
            for d in range(dimension):
                f.write(' %g' % data[n, d])
        f.write('\n')


def _idf( tcDataset ):
    data = tcDataset.getData()
    numberOfDocuments = data.shape[0]
    idf = np.log(1.0 * numberOfDocuments / np.maximum(1, np.sum(data > 0, axis=0)))
    return idf


def id2tfidf(idDataset, docTimeline, dimension=None, weighted=False):

    if dimension == None:
        # assume 'words' are numbered from 0 to dimension-1
        data = idDataset.getData()
        dimension = np.max(data) + 1

    # Compute inverse document frequency
    tcDataset = aggregate(idDataset, docTimeline, aggregator=pinocchIO.utils.aggregator.frequency.termCount, **{'dimension':dimension})
    idf = _idf(tcDataset)

    # Compute term frequency * inverse document frequency
    tfidfDataset = aggregate(idDataset, docTimeline, aggregator=pinocchIO.utils.aggregator.frequency.tfidf, **{'dimension': dimension, 'idf': idf, 'weighted': weighted})

    return tfidfDataset    