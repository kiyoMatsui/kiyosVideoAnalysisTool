 
#!/bin/bash

valgrind --tool=massif --time-unit=B  ./../build/bin/kiyosVideoAnalysisTool

###use massif-visualizer to view output file.
