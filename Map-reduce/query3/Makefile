done: root
	@echo "Done, Result is written in result3.txt" 

root:
	@echo "Please Wait.."
	@python3 mapper.py access_log.txt | sort | python3 reducer.py > result3.txt

clean:
	rm result1.txt