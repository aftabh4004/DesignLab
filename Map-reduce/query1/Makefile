done: root
	@echo "Done, Result is written in result1.txt" 

root:
	@echo "Please Wait.."
	@python3 mapper.py access_log.txt | sort | python3 reducer.py > result1.txt

clean:
	rm result1.txt