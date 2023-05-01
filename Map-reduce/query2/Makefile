done: root
	@echo "Done, Result is written in result2.txt" 

root:
	@echo "Please Wait.."
	@python3 mapper.py access_log.txt | sort | python3 reducer.py > result2.txt

clean:
	rm result1.txt
