"""
Method to sort a dictionary by name (case insensitive)
"""

def sorted_by_name(dictionary):
	return sorted( dictionary, cmp=case_insensitive_comparator, key=lambda item: item['name'] )

def case_insensitive_comparator (item1 , item2):
	if item1.lower() > item2.lower():
		return 1
	return -1