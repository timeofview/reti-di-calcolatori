from threading import Thread
import os.path,subprocess
from subprocess import STDOUT,PIPE
from datetime import datetime


def input():
    f=open("risultati.txt","a")
    print("----------------")
    print("||Initializing||")
    print("----------------")
    now = datetime.now()
    programName='Client.java'
    compile_java(programName)
    prima = '1'
    seconda = '2'
    completa = prima+'\n'+seconda

    for i in range(1000):
        compiled_now = datetime.now()
        execute_java(programName, completa)
        executed_now = datetime.now()
        f.write(str(executed_now-compiled_now))
        f.write('\n')
    
    
    print("  ||")
    print("  ||")
    print("  \/")
    print("-------------------------------------")
    print("||Compile Time||", compiled_now-now,"||")
    print("||Execute Time||", executed_now-compiled_now,"||")
    print("||Total   Time||", executed_now-now,"||")
    print("-------------------------------------")
  

    f.close()

def compile_java(java_file):
    print("-----------------------")
    print("||Starting to Compile||")
    print("-----------------------")
    subprocess.check_call(['javac', java_file])
    print("------------")
    print("||Compiled||")
    print("------------")
    print("  ||")
    print("  ||")
    print("  \/")

def execute_java(java_file, stdin):
    java_class,ext = os.path.splitext(java_file)
    print(java_class)
    cmd = ['java', java_class, 'localhost', '54321', '9550.txt']
    proc = subprocess.Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=STDOUT)
    stdout,stderr = proc.communicate(stdin.encode())
    print (stdout.decode())




Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 
Thread(target = input).start() 