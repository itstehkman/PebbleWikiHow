import sys
import json
import requests
from fuzzywuzzy import process
from bs4 import BeautifulSoup as bs
import re

def is_step(tag):
	return tag('li') and not tag.previous_element('ul')

query = sys.argv[1].lower()
r = requests.get('http://wikihow.com/api.php?action=query&list=search&srsearch=%s&utf8=&format=json' % query)
search_results = r.json()
search_results = [results["title"] for results in search_results["query"]["search"]]

# remove how to from query
s_query = query.replace("how to ", "")
best_title = process.extractOne(s_query, search_results)[0].encode('ascii','replace')
#print best_title
if not best_title:
	sys.exit(100)

r = requests.get('http://wikihow.com/api.php?action=parse&page=%s&section=1&prop=text&format=json' % best_title)
steps = r.json()
steps = steps["parse"]["text"]["*"]
#steps = re.sub(r"</*b>", "", steps) # remove bold text to avoid bs issues
steps = re.sub(r"(</a>)|(<a href[^>]+>)", "", steps) # remove any a href urls
steps = re.sub(r"<su(b|p)[^>]*>((?:.|\n)*?)</su\1>", "", steps) # remove super/subscript
steps = re.sub(r"</*(i|b)>", "", steps) # remove italics and bold
soup = bs(steps, 'html.parser')
[s.extract() for s in soup(['ul'])] # remove bold, unnumbered lists, super/subscript
#print soup.prettify()
#sys.exit(0)
method_1 = soup.find('ol')#.contents
#print method_1
#sys.exit(0)
print json.dumps([best_title] + ([step.contents[0].encode('utf-8').strip() for step in method_1.find_all('li')]))
sys.stdout.flush()
sys.exit(0)
