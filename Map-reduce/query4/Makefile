done: root
	@echo "Done, Result is written in result4.txt" 

root:
	@echo "Please Wait.."
	@python3 mapper.py access_log.txt | sort -k 1,1 | python3 reducer.py > result4.txt

clean:
	rm result4.txt