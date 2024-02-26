import matplotlib.pyplot as plt

# Data from the prompt
cores = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
speedup = [1.0, 1.2847712820174104, 2.9067946267760547, 3.876084576717799, 4.9094389286438584, 5.860871230206207, 6.78899125789732, 7.6858624884785, 8.765929014160143, 9.756296503151358, 10.663269572681768, 11.698823399831952, 10.568728856003142, 11.656746577162805, 12.35736112459507]
times = [155.923638, 121.362954, 53.641092, 40.227099, 31.759971, 26.604174, 22.967129, 20.287071, 17.787463, 15.981847, 14.622498, 13.328147, 14.753301, 13.376257, 12.617875]

# Calculate speedup
speedup = [times[0] / time for time in times]
print(speedup)
# Plot wall time vs cores
plt.figure(figsize=(8, 6))
plt.plot(cores, times, marker='o', label='Wall Time (s)')
plt.xlabel('Number of Cores')
plt.ylabel('Wall Time (s)')
plt.title('Wall Time vs. Cores')
plt.grid(True)
plt.legend()

# Plot speedup vs cores
plt.figure(figsize=(8, 6))
plt.plot(cores, speedup, marker='o', label='Speedup')
plt.xlabel('Number of Cores')
plt.ylabel('Speedup (S)')
plt.title('Speedup vs. Cores')
plt.grid(True)
plt.legend()

# Display both plots
plt.show()

# export OMP_NUM_THREADS=1
# ./matrix_mult

import matplotlib.pyplot as plt

# Data from the prompt

cores = [1, 2, 4, 8]
times = [155.923638, 121.362954, 40.227099,  20.287071]
speedup= [1.0, 1.2847712820174104, 3.876084576717799, 7.6858624884785]

# cores = [1, 2, 4, 8]
# times = [254.155101, 232.454903, 225.065215, 210.548985]
# speedup = [1.0, 1.093352292078778, 1.1292509195612481, 1.207106750004043]

# Calculate speedup
speedup = [times[0] / time for time in times]
print(speedup)
# # Plot wall time vs cores
# plt.figure(figsize=(8, 6))
# plt.plot(cores, times, marker='o', label='Wall Time (s)')
# plt.xlabel('Number of Cores')
# plt.ylabel('Wall Time (s)')
# plt.title('Wall Time vs. Cores')
# plt.grid(True)
# plt.legend()

# # Plot speedup vs cores
# plt.figure(figsize=(8, 6))
# plt.plot(cores, speedup, marker='o', label='Speedup')
# plt.xlabel('Number of Cores')
# plt.ylabel('Speedup (S)')
# plt.title('Speedup vs. Cores')
# plt.grid(True)
# plt.legend()

# # Display both plots
# plt.show()

cores = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
times = [155.923638, 121.362954, 53.641092, 40.227099, 31.759971, 26.604174, 22.967129, 20.287071, 17.787463, 15.981847, 14.622498, 13.328147, 14.753301, 13.376257, 12.617875]
speedup = [1.0, 1.2847712820174104, 2.9067946267760547, 3.876084576717799, 4.9094389286438584, 5.860871230206207, 6.78899125789732, 7.6858624884785, 8.765929014160143, 9.756296503151358, 10.663269572681768, 11.698823399831952, 10.568728856003142, 11.656746577162805, 12.35736112459507]
