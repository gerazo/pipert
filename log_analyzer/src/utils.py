from rdp import rdp


def flatten_list(list_of_lists):
    """Transforming a list of lists into one list with all the elements

    Args:
        list_of_lists: A list of lists of elements

    Returns:
        A list of all the elements of the inner lists
    """

    return [item for sublist in list_of_lists for item in sublist]


def calc_avg(lst):
    """ Calculate the average of numbers in a list

    Args:
        lst: A list of numbers

    Returns:
        An integer of the average or zero if the list is empty
    """

    if not lst:
        return 0
    else:
        return sum(lst) / len(lst)


def reduce_points_n_extract_y_axis(points):
    """ Apply the Ramer–Douglas–Peucker algorithm and extracting the x-axis of the points

    Args:
        points: A list of tuple of integers or real numbers

    Returns:
        A list of integers or reals of the x-axis
    """

    minimized_points = rdp(points, epsilon=0.5)
    ret_points = [None] * 10
    for point in minimized_points:
        index = int((point[0] % 10) - 1)
        ret_points[index] = point[1]

    return ret_points
