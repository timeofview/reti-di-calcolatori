import os.path,subprocess
from subprocess import STDOUT,PIPE
from datetime import datetime


def main():
    print("----------------")
    print("||Initializing||")
    print("----------------")
    now = datetime.now()
    programName='Hi.java'
    compile_java(programName)
    compiled_now = datetime.now()
    prima = input("Inserisci prima riga: ")
    seconda = input("Inserisci seconda riga: ")
    completa = prima+'\n'+seconda
    execute_java(programName, completa)
    executed_now = datetime.now()
    print("  ||")
    print("  ||")
    print("  \/")
    print("-------------------------------------")
    print("||Compile Time||", compiled_now-now,"||")
    print("||Execute Time||", executed_now-compiled_now,"||")
    print("||Total   Time||", executed_now-now,"||")
    print("-------------------------------------")

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
    cmd = ['java', java_class, input("Inserisci IP: "), input("Inserisci Porta: "), input("Inserisci File: ")]
    proc = subprocess.Popen(cmd, stdin=PIPE, stdout=PIPE, stderr=STDOUT)
    stdout,stderr = proc.communicate(stdin.encode())
    print (stdout.decode())

if __name__ == "__main__":
    main()
