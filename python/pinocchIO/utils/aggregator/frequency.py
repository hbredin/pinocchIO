import numpy as np
import pyfusion.normalization.L1 as L1Norm
import pyfusion.normalization.L2 as L2Norm
import pyfusion


def termCount(dataset, **kwargs):
    """
    Must be provided with a 'dimension' parameter.
    Optional 'weighted' parameter can be provided (default is False)
             If 'weighted', each index is weighted by the duration of the corresponding time range
    """
    
    if 'dimension' in kwargs:
        dimension = kwargs['dimension']
    else:
        raise ValueError('Unknown dimension.')
    
    if 'weighted' in kwargs:
        weighted = kwargs['weighted']
    else:
        weighted = False
    
    # return a vector full of zero in case dataset is empty
    if dataset.isEmpty():
        return np.zeros((1, dimension), dtype=np.float32)
    
    if dataset.getDimension() != 1:
        raise ValueError('termCount aggregator only applies to IDs (1-dimensional vector) datasets')

    iC = np.zeros((1, dimension), dtype=np.float32)    
    for t, timerange in enumerate(dataset.getTimeline()):
        if weighted:
            count = timerange.getDuration().total_seconds()
        else:
            count = 1
        for v, value in enumerate(dataset[t]):
            iC[0, value] += count
    
    return iC


def termFrequency(dataset, **kwargs):
    return L1Norm.normalize(termCount(dataset, **kwargs))


def tfidf(dataset, **kwargs):
    
    if 'idf' in kwargs:
        inverseDocumentFrequency = kwargs['idf']
    else:
        raise ValueError('Unknown inverse document frequency.')
    
    return L2Norm.normalize(inverseDocumentFrequency * termFrequency(dataset, **kwargs))
    