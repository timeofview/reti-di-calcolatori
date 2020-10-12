import matplotlib.pyplot as plt 

sum = float(0)

# Array asse x
assex = []

# Array asse y
assey = []

#Apro file
f = open("risultati.txt", "r")

#Leggo linee
f1 = f.readlines()
i = 0
#round(number, ndigits)
for c in f1:
#	assey.append(int(c[-7:-1])/1000)
#	if (float(c[-9:-1])*1000>220):
#		continue
	if (c[-9:-1]==''):
		continue
	if (float(c[-9:-1])*1000>1300):
		continue
	assey.append(float(c[-9:-1])*1000)
	sum+=float(c[-9:-1])*1000

#	if (int(c[-7:-1])/1000>800):
#		continue
#	assey.append(int(c[-7:-1])/1000)
#	sum+=int(c[-7:-1])
	assex.append(i)
	i+=1
f.close()

# Label for bars 
# labelx = ['one', 'two', 'three', 'four', 'five'] 
#assey.sort()  
# Plotting
plt.bar(assex, assey, 
        width = 0.8, color = ['red', 'green']) 
#plt.bar(assex, assey, tick_label = labelx, 
        #width = 0.8, color = ['red', 'green', 'blue']) 
  
# Nome asse x
plt.xlabel('Asse x -> Numero Richiesta') 

# Nome asse y
plt.ylabel('Asse y -> Tempo Richiesta (ms)') 

# Titolo plot
plt.title('Server con 500 file gestito tramite array paralleli') 

print("Media: ", sum/len(assey))  
print("Max value: ", max(assey)) 
print("Min value: ", min(assey)) 

# Show the plot 
plt.show() 