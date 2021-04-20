from rdp import rdp


def flatten_list(listOfLists):
    return [item for sublist in listOfLists for item in sublist]


def calc_avg(list):
    if(len(list) == 0):
        return 0
    else:
        return sum(list)/len(list)

def reduce_points_n_extract_x_axis(points):
    minimized_points = rdp(points, epsilon=0.5)
    ret_points = [None] * 10
    for point in minimized_points:
        index = int((point[0] % 10) - 1)
        ret_points[index] = point[1]

    return ret_points
