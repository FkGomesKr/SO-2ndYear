# Operating Systems
Task Orchestrator - Various clients can send tasks to the main server/orchestrator to execute those tasks concurrently 
  Init Orchestrator - ./bin/orchestrator ouput_folder 3 SJF
  Possible Tasks Examples - ./bin/client execute -u 500 "ls"
                            ./bin/client execute 300 -p "cat src/client.c | grep "mk" | wc -l"
                            ./bin/client Status
  Makefile: Compile - make
            Clean - make clean
