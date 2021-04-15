def flatten_list(listOfLists):
    return [item for sublist in listOfLists for item in sublist]
def calc_avg(list):
    if(len(list)==0):
        return 0
    else:
        return sum(list)/len(list)
