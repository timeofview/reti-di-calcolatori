import matplotlib.pyplot as plt 

sum = float(0)

# Array asse x
assex = [1,2,3,4]

# Array asse y
assey = [554.310,708.507,554.310,564.283]

# Label for bars 
labelx = ['linearSearch', 'binarySearch', 'ArrayList', 'map'] 
#assey.sort()  
# Plotting
#plt.bar(assex, assey, 
        #width = 0.8, color = ['red', 'green']) 
plt.bar(assex, assey, tick_label = labelx, 
        width = 0.8, color = ['red', 'green', 'blue', 'yellow']) 
  
# Nome asse x
plt.xlabel('Asse x -> Data Structure') 

# Nome asse y
plt.ylabel('Asse y -> Tempo Medio Richiesta (ms)') 

# Titolo plot
plt.title('Medie a confronto') 

# Show the plot 
plt.show() 