file_prefix = "slab_1mps_pbrt"
file_extension = ".pbrt"
start_index = 120
end_index = 180

for i in range(start_index, end_index + 1):
    filename = file_prefix + str(i).zfill(5) + "_texture" + file_extension
    with open(filename, 'r+') as file:
        # content = file.read()
        file.seek(0)  # Move the file pointer to the beginning of the file
        file.write(" ")  # Write the content back to the file
        # file.truncate()  # Remove any remaining content after the updated content