import numpy as np


def first(dataset, **kwargs):
    data = dataset[0]
    return np.array(data, ndmin=2)


def last(dataset, **kwargs):
    data = dataset[-1]
    return np.array(data, ndmin=2)


def max(dataset, **kwargs):
    data = dataset.getData()
    return np.array(np.max(data, axis=0), ndmin=2)


def min(dataset, **kwargs):
    data = dataset.getData()
    return np.array(np.min(data, axis=0), ndmin=2)


def sum(dataset, **kwargs):
    data = dataset.getData()
    return np.array(np.sum(data, axis=0), ndmin=2)


def median(dataset, **kwargs):
    data = dataset.getData()
    return np.array(np.median(data, axis=0), ndmin=2)


def mean(dataset, **kwargs):
    data = dataset.getData()
    return np.array(np.mean(data, axis=0), ndmin=2)


def average(dataset, **kwargs):
    """
    Weighted average of vector entries.
    Each entry is weighted with the duration of the time range it belongs to.
    """
    timeline = dataset.getTimeline()
    durations = [timerange.getDuration().total_seconds() for t, timerange in enumerate(timeline)]
    
    weights = []
    for n, number in enumerate(dataset.getNumber()):
        for i in range(number):
            weights.append(durations[n])
    
    data = dataset.getData()
    return np.array(np.average(data, axis=0, weights=weights), ndmin=2)


