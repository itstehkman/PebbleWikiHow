import sys
import json
import requests
from fuzzywuzzy import process
from bs4 import BeautifulSoup as bs
import re

def throw_err():
	print
	sys.stdout.flush()
	sys.exit(0)

if len(sys.argv) < 2:
	throw_err()
query = sys.argv[1].lower()
if not query:
	throw_err()
r = requests.get('http://wikihow.com/api.php?action=query&list=search&srsearch=%s&utf8=&format=json' % query)
search_results = r.json()
search_results = [results["title"] for results in search_results["query"]["search"]]
if not search_results:
	throw_err()

# remove how to and how do i from query
s_query = query.replace("how to ", "").replace("how do i ", "")
best_title = process.extractOne(s_query, search_results)[0].encode('ascii','replace')
#print best_title
if not best_title:
	throw_err()

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
if not method_1:
	throw_err()
#print method_1
#sys.exit(0)
print json.dumps([best_title] + ([step.contents[0].encode('utf-8').strip() for step in method_1.find_all('li')]))
sys.stdout.flush()
sys.exit(0)
