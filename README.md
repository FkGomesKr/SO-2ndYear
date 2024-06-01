# Operating Systems
Task Orchestrator - Various cliente can send tasks to the main server/orchestrator
  Init Orchestrator - ./bin/orchestrator ouput_folder 3 SJF
  Possible Tasks Examples - ./bin/client execute -u 500 "ls"
                            ./bin/client execute 300 -p "cat src/client.c | grep "mk" | wc -l"
                            ./bin/client Status

