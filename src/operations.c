#include "../lib/operations.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int
fs_mkdir(file_system *fs, char *path)
{
    int count = 0;
	for (int i = 0; i < strlen(path); i++)
	{
		if (path[i] == '/')
		{
			count++;
		}
		
	}

	char *dir_name = strrchr(path, '/');
    if (dir_name != NULL) {
        dir_name++; 
    } 
	else{
        return -1; 
    }

    for(int i = 0; i < fs->s_block->num_blocks; ++i) {
        if(fs->inodes[i].n_type == 2 && strcmp(fs->inodes[i].name, dir_name) == 0) {
            return -1;
        }
    }

	if (fs->inodes[count-1].n_type != 2)
	{
		return -1;
	}
	
	
    int new_dir_inode_num = find_free_inode(fs);
    if (new_dir_inode_num == -1){
        return -1;
    }

    inode* new_dir_inode = &fs->inodes[new_dir_inode_num];
    new_dir_inode->n_type = 2;
    strncpy(new_dir_inode->name, dir_name, NAME_MAX_LENGTH);

	for (int i = 0; i < sizeof(fs->inodes[count-1].direct_blocks); i++)
	{
		if (fs->inodes[count-1].direct_blocks[i] == NULL)
		{
			fs->inodes[count-1].direct_blocks[i] = new_dir_inode_num;
			break;
		}
		
	}

	if (new_dir_inode_num == 1)
	{
		fs->inodes[0].direct_blocks[0] = new_dir_inode_num;
	}
	
	
	
    new_dir_inode->parent = count-1; 
    memset(new_dir_inode->direct_blocks, -1, DIRECT_BLOCKS_COUNT * sizeof(int));

    new_dir_inode->direct_blocks[0] = fs->root_node;

    fs->free_list[new_dir_inode_num] = 0;
    fs->s_block->free_blocks--;

    return 0;
}

int
fs_mkfile(file_system *fs, char *path_and_name)
{
	 int count = 0;
	for (int i = 0; i < strlen(path_and_name); i++)
	{
		if (path_and_name[i] == '/')
		{
			count++;
		}
		
	}

	char *dir_name = strrchr(path_and_name, '/');
    if (dir_name != NULL) {
        dir_name++; 
    } 
	else{
        return -1; 
    }

    
    for(int i = 0; i < fs->s_block->num_blocks; ++i) {
        if(fs->inodes[i].n_type == 1 && strcmp(fs->inodes[i].name, dir_name) == 0) {
            return -2;
        }
    }

	if (fs->inodes[count-1].n_type != 2)
	{
		return -1;
	}
	
	
    int new_dir_inode_num = find_free_inode(fs);
    if (new_dir_inode_num == -1){
        return -1;
    }

    inode* new_dir_inode = &fs->inodes[new_dir_inode_num];
    new_dir_inode->n_type = 1;
    strncpy(new_dir_inode->name, dir_name, NAME_MAX_LENGTH);

	for (int i = 0; i < sizeof(fs->inodes[count-1].direct_blocks); i++)
	{
		if (fs->inodes[count-1].direct_blocks[i] == -1)
		{
			fs->inodes[count-1].direct_blocks[i] = new_dir_inode_num;
			break;
		}
		
	}

	if (new_dir_inode_num == 1)
	{
		fs->inodes[0].direct_blocks[0] = new_dir_inode_num;
	}
	
	
	
    new_dir_inode->parent = count-1; 
    memset(new_dir_inode->direct_blocks, -1, DIRECT_BLOCKS_COUNT * sizeof(int)); 

    fs->free_list[new_dir_inode_num] = 0;
    fs->s_block->free_blocks--;

    return 0;
}

char *concat(const char *s1, const char *s2) //To solve "Trace/BPT trap:5" error
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1); 
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}


char *
fs_list(file_system *fs, char *path)
{
	inode *current_inode;
	char *list = malloc(sizeof(inode)*100);

	if (strcmp(path, "/") == 0)
	{
		current_inode = &fs->inodes[fs->root_node];
	}
	else
	{
		char* token = strtok(path, "/");
		if (token == NULL){
		    current_inode = NULL;
		}
		else
		{
			current_inode = &fs->inodes[fs->root_node];
		}
		
		
        while (token != NULL)
		{
			int found = 0;
			for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
			{
				if (current_inode->direct_blocks[i] != -1)
				{
					inode *next_directory = &fs->inodes[current_inode->direct_blocks[i]];
					if (strcmp(next_directory->name, token) == 0)
					{
						current_inode = next_directory;
						found = 1;
						break;
					}
				}
			}
			if (!found)
			{
				current_inode = NULL; 
			}
			token = strtok(NULL, "/");
		}
	}

	
	for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
	{
		int block_number = current_inode->direct_blocks[i];

		if (block_number != -1)
		{
			inode *next_inode = &fs->inodes[block_number];

			if (next_inode->n_type == 1)
			{
				char *new_entry = concat("FIL ", next_inode->name);
				if (list == NULL)
				{
					list = new_entry;
				}
				else
				{
					list = concat(list, new_entry);
					free(new_entry);
				}
				list = concat(list, "\n");
			}
			else if (next_inode->n_type == 2)
			{
				char *new_entry = concat("DIR ", next_inode->name);
				if (list == NULL)
				{
					list = new_entry;
				}
				else
				{
					list = concat(list, new_entry);
					free(new_entry);
				}
				list = concat(list, "\n");
			}
		}
	}
    
	return list;
}

int
fs_writef(file_system *fs, char *filename, char *text)
{
	inode *file_node = malloc(sizeof(inode));
	if (strcmp(filename, "/") == 0)
	{
		file_node = &fs->inodes[fs->root_node];
	}
	else
	{
		char* token = strtok(filename, "/");
		if (token == NULL){
		    file_node = NULL;
		}
		else
		{
			file_node = &fs->inodes[fs->root_node];
		}
		
		
        while (token != NULL)
		{
			int found = 0;
			for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
			{
				if (file_node->direct_blocks[i] != -1)
				{
					inode *next_directory = &fs->inodes[file_node->direct_blocks[i]];
					if (strcmp(next_directory->name, token) == 0)
					{
						file_node = next_directory;
						found = 1;
						break;
					}
				}
			}
			if (!found)
			{
				file_node = NULL; 
			}
			token = strtok(NULL, "/");
		}
	}

	if (file_node == NULL || file_node->n_type != 1)
	{
		return -1; 
	}

	int text_len = strlen(text);
	int remaining_len = text_len; 
	int text_offset = 0;		  
	
	for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
	{
		int block_num = file_node->direct_blocks[i];

		if (block_num == -1)
		{
			if (fs->s_block->free_blocks <= 0)
			{
				return -2;
			}
			else
			{
				for (int j = 0; j < fs->s_block->num_blocks; j++)
				{
					if (fs->free_list[j] == 1)
					{
						block_num = j;
						break;
					}
					
				}
				
			}
			
			file_node->direct_blocks[i] = block_num;
			fs->free_list[block_num] = 0;
			fs->s_block->free_blocks--;
		}

		data_block *block = &(fs->data_blocks[block_num]);
		int can_write = MIN(remaining_len, BLOCK_SIZE - block->size);
		memcpy(&(block->block[block->size]), &(text[text_offset]), can_write);

		block->size = block->size + can_write;
		file_node->size = file_node->size + can_write;
		remaining_len = remaining_len - can_write;
		text_offset = text_offset + can_write;

		if (remaining_len == 0)
		{
			break;
		}
	}

	if (remaining_len > 0)
	{
		return -2; 
	}

	return text_len;
}

uint8_t *
fs_readf(file_system *fs, char *filename, int *file_size)
{
    inode *current_inode = &(fs->inodes[fs->root_node]);

    char *token = strtok(filename, "/");
    while (token != NULL)
    {
        if (current_inode->n_type != directory)
        {
            *file_size = 0;
            return NULL;
        }

        int found = 0;
        for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
        {
            if (current_inode->direct_blocks[i] != -1)
            {
                inode *child_inode = &(fs->inodes[current_inode->direct_blocks[i]]);
                if (strcmp(child_inode->name, token) == 0)
                {
                    current_inode = child_inode;
                    found = 1;
                    break;
                }
            }
        }

        if (!found)
        {
            *file_size = 0;
            return NULL;
        }

        token = strtok(NULL, "/");
    }

    if (current_inode->n_type != reg_file)
    {
        *file_size = 0;
        return NULL;
    }

    uint8_t *buffer = (uint8_t *)malloc(current_inode->size);
    if (buffer == NULL)
    {
        *file_size = 0;
        return NULL;
    }

    int offset = 0;
    for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
    {
        int block_num = current_inode->direct_blocks[i];
        if (block_num != -1)
        {
            data_block *dblock = &(fs->data_blocks[block_num]);
            memcpy(buffer + offset, dblock->block, dblock->size);
            offset += dblock->size;
        }
    }

    if (offset == 0)
    {
        free(buffer);
        *file_size = 0;
        return NULL;
    }

	buffer[offset] = '\0';
    *file_size = offset;
    return buffer;
}


int
fs_rm(file_system *fs, char *path)
{
	char *dq = malloc(sizeof(data_block)*25);
	dq[1] = path;
    inode *current = &(fs->inodes[fs->root_node]);

    char *token = strtok(path, "/");
    while (token != NULL)
    {
        if (current->n_type != 2)
        {
            return -1;
        }
        int found = 0;
        for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
        {
            if (current->direct_blocks[i] != -1)
            {
                inode *child = &(fs->inodes[current->direct_blocks[i]]);
                if (strcmp(child->name, token) == 0)
                {
                    current = child;
                    found = 1;
                    break;
                }
            }
        }

        if (!found)
        {
            return -1;
        }

        token = strtok(NULL, "/");
    }

    if (current->n_type == 2)
    {
        for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
        {
            if (current->direct_blocks[i] != -1)
            {
                inode *child = &(fs->inodes[current->direct_blocks[i]]);
                char *child_path = (char *)malloc(strlen(path) + NAME_MAX_LENGTH + 2);
                if (child_path == NULL)
                {
                    return -1; 
                }
                strcpy(child_path, path);
                strcat(child_path, "/");
                strcat(child_path, child->name);

                int status = fs_rm(fs, child_path); 
                free(child_path);

                if (status != 0)
                {
                    return -1; 
                }
            }
        }
    }
    inode *parent = &(fs->inodes[current->parent]);
    for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
    {
        if (parent->direct_blocks[i] == current - fs->inodes)
        {
            parent->direct_blocks[i] = -1;
        }
    }

    current->n_type = 3;
    for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
    {
        if (current->direct_blocks[i] != -1)
        {
            fs->data_blocks[current->direct_blocks[i]].size = 0; 
            fs->free_list[current->direct_blocks[i]] = 1;       
            current->direct_blocks[i] = -1;                     
        }
    }
    free(dq);
    return 0; 
}

int
fs_import(file_system *fs, char *int_path, char *ext_path)
{
	return -1;
}

int
fs_export(file_system *fs, char *int_path, char *ext_path)
{
    inode *current_inode = &(fs->inodes[fs->root_node]);

    char *token = strtok(int_path, "/");
    while (token != NULL)
    {
        if (current_inode->n_type != directory)
        {
            return -1;
        }

        int found = 0;
        for (int i = 0; i < DIRECT_BLOCKS_COUNT; i++)
        {
            if (current_inode->direct_blocks[i] != -1)
            {
                inode *child_inode = &(fs->inodes[current_inode->direct_blocks[i]]);
                if (strcmp(child_inode->name, token) == 0)
                {
                    current_inode = child_inode;
                    found = 1;
                    break;
                }
            }
        }

        if (!found)
        {
            return -1;
        }
        token = strtok(NULL, "/");
    }

    if (current_inode->n_type != reg_file)
    {
        return -1;
    }

    FILE *ext_file = fopen(ext_path, "wb");
    if (ext_file == NULL)
    {
        return -1;
    }

    for (int i = 0; i < DIRECT_BLOCKS_COUNT; ++i)
    {
        int block_number = current_inode->direct_blocks[i];
        if (block_number != -1)
        {
            data_block *db = &(fs->data_blocks[block_number]);
            fwrite(db->block, 1, db->size, ext_file);
        }
    }

    fclose(ext_file);

    return 0;
}
