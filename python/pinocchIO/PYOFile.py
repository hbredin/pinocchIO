import h5py
import PYODataset, PYOTimeline

def _recursive_add_h5dataset(listOfDatasets, object):
    if type(object) == h5py.highlevel.Dataset:
        listOfDatasets.append(object.name)
    if type(object) == h5py.highlevel.Group or type(object) == h5py.highlevel.File:
        for i in object.keys():
            _recursive_add_h5dataset(listOfDatasets, object[i])


class PYOFile(object):
    """
    pinocchIO file
    """
    
    def __init__(self, path):
        """
        Open pinocchIO file at path
        """
        super(PYOFile, self).__init__()
        self.h5file = h5py.File(path, 'r')
    
    
    def close(self):
        """
        Close pinocchIO file
        """
        self.h5file.close()
    
    
    def datasets(self):
        """
        Get list of datasets
        """
        h5datasets = []
        # get all HDF5 dataset paths contained in /dataset
        _recursive_add_h5dataset(h5datasets, self.h5file['dataset'])
        datasets = []
        # each pinocchIO dataset is made of two datasets PATH/data and PATH/link
        for h5d in h5datasets:
            # get rid of PATH/link entries 
            if h5d.rsplit('/', 1)[1] == 'data':
                # remove /data suffix and /dataset prefix
                datasets.append('/' + h5d.rsplit('/', 1)[0].split('/', 2)[2])
        return datasets
    
    
    def timelines(self):
        """
        Get list of timelines
        """
        h5timelines = []
        # get all HDF5 dataset paths contained in /timeline
        _recursive_add_h5dataset(h5timelines, self.h5file['timeline'])
        timelines = []
        for h5d in h5timelines:
            # remove /timeline prefix
            timelines.append('/' + h5d.split('/', 2)[2])
        return timelines
    
    
    def getDataset(self, path):
        """
        Load dataset at internal path
        """
        return PYODataset._fromFile(self, path)
    
    
    def getTimeline(self, path):
        """
        Load timeline at internal path
        """
        return PYOTimeline._fromFile(self, path)
    
    
