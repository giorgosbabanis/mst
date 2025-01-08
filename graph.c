#ifndef __GRAPH_C
#define __GRAPH_C

#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <libgen.h>
#include <limits.h>

#include "omp.c"


// 4 Bytes ID per vertex, without weights on edges or vertices
struct ll_400_graph
{
	unsigned long vertices_count;
	unsigned long edges_count;
	unsigned long* offsets_list;
	unsigned int* edges_list;
};

// 4 Bytes ID per vertex, 4 Bytes weight per edge, with no weight on vertices
// Each edge has two unsigned int elements in the edges_list, 
// The first one is the destination/source, and 
// The second one is the weight
struct ll_404_graph
{
	unsigned long vertices_count;
	unsigned long edges_count;
	unsigned long* offsets_list;
	unsigned int* edges_list;
};

// 8 Bytes IDs per vertex, without weights on edges or vertices
struct ll_800_graph
{
	unsigned long vertices_count;
	unsigned long edges_count;
	unsigned long* offsets_list;
	unsigned long* edges_list;
};

void print_ll_400_graph(struct ll_400_graph* ret)
{
	printf("\n|V|: %'20lu\n|E|: %'20lu\n", ret->vertices_count, ret->edges_count);
	printf("First offsets: ");
	for(unsigned int v=0; v<min(ret->vertices_count + 1, 20); v++)
		printf("%lu, ", ret->offsets_list[v]);
	if(ret->vertices_count > 20)
	{
		printf("...\nLast offsets: ... ");
		for(unsigned int v = ret->vertices_count - 20; v <= ret->vertices_count; v++)
			printf(", %lu", ret->offsets_list[v]);
	}

	if(ret->edges_list)
	{
		printf("\nFirst edges: ");
		for(unsigned long e=0; e<min(ret->edges_count, 20); e++)
			printf("%u, ", ret->edges_list[e]);
		if(ret->edges_count > 20)
		{
			printf(" ...\nLast edges: ... ");
			for(unsigned long e = ret->edges_count - 20; e < ret->edges_count; e++)
				printf(", %u", ret->edges_list[e]);
		}
	}

	printf("\n\n");

	return;
}

char* get_shm_graph_name(char* file_name)
{
	assert(file_name != NULL);

	char* cfn = NULL;
	if(access(file_name, F_OK) == 0 )
		// for .txt graphs
		cfn = file_name;
	else
	{
		// for unweighted WebGraph graphs with .graph suffix
		cfn = malloc(PATH_MAX);
		assert(cfn != NULL);
		sprintf(cfn, "%s.graph", file_name);

		if(access(cfn, F_OK) != 0)
		{
			// for weighted WebGraph graphs with .labels suffix
			sprintf(cfn, "%s.labels", file_name);
			int ra = access(cfn, F_OK);
			assert(ra == 0);
		}
	}

	char* abs_file_name = malloc(PATH_MAX);
	assert(abs_file_name != NULL);
	{
		char* ret = realpath(cfn, abs_file_name);
		assert(ret != NULL && ret == abs_file_name);
	}

	int len = strlen(abs_file_name);
	char* ret = malloc(len + 16);
	assert(ret != NULL);

	sprintf(ret, "ll_graph%s", abs_file_name);
	for(int i = 8; i <= 8 + len; i++)
		if(ret[i] == '/')
			ret[i] = '_';

	free(abs_file_name);
	abs_file_name = NULL;

	if(cfn != file_name)
	{
		free(cfn);
		cfn = NULL;
	}

	return ret;
}

struct ll_400_graph* get_shm_ll_400_graph(char* file_name, unsigned long vertices_count, unsigned long edges_count)
{
	char* shm_name = get_shm_graph_name(file_name);
	printf("shm_name: %s\n", shm_name);
	unsigned long graph_size = (2 + vertices_count + 1) * sizeof(unsigned long) + edges_count * sizeof(unsigned int);

	struct ll_400_graph* g = NULL;
	int shm_fd = shm_open(shm_name, O_RDONLY, 0);
	if(shm_fd > 0)
	{
		printf("Shared memory file exists.\n");
		unsigned long* ul_graph = (unsigned long*)mmap(NULL, graph_size, PROT_READ, MAP_PRIVATE, shm_fd, 0);
		if(ul_graph == MAP_FAILED)
		{
			printf("Couldn't get graph -> mmap error : %d, %s\n", errno, strerror(errno) );
			assert (ul_graph != MAP_FAILED);
		}
		close(shm_fd);
		shm_fd = -1;
		
		g = malloc(sizeof(struct ll_400_graph));
		assert(g != NULL);
		g->vertices_count = ul_graph[0];
		g->edges_count = ul_graph[1];
		g->offsets_list = &ul_graph[2];
		g->edges_list = (unsigned int*)(&ul_graph[ 2 + ul_graph[0] + 1 ]);
	}

	// Release mem
		free(shm_name);
		shm_name = NULL;

	return g;
}

/*
	`flags`:
		bit 0: 
			Read from storage. Do not use the copy in /dev/shm (if it exists).
		
		bit 31: 
			Will be set by the function if the graph has been mapped from a copy in /dev/shm. 
			This flag should be used for releasing the graph. If it is set, `release_shm_ll_400_graph()` should be called.
			Otherwise, `release_numa_interleaved_ll_400_graph()` should be called.
*/
struct ll_400_graph* get_ll_400_txt_graph(char* file_name, unsigned int* flags)
{
	// Checks 
		assert(flags != NULL);

		if(access(file_name, F_OK) != 0)
		{
			printf("Error: file \"%s\" does not exist\n",file_name);
			return NULL;
		}

	// Reading vertices and edges count and graph size
		unsigned long vertices_count = 0;
		unsigned long edges_count = 0;	
		{
			char temp[512];
			sprintf(temp, "head -n1 %s", file_name);
			FILE *fp = popen(temp, "r");
			int ret = fscanf(fp, "%lu", &vertices_count);
			assert(ret == 1);
			pclose(fp);
			printf("Vertices: %'lu\n",vertices_count);

			sprintf(temp, "head -n2 %s | tail -n1", file_name);
			fp = popen(temp, "r");
			ret = fscanf(fp, "%lu", &edges_count);
			assert(ret == 1);
			pclose(fp);
			printf("Edges: %'lu\n",edges_count);
		}

	// Check if the graph exists in /dev/shm
		if((*flags & 1U<<0) == 0)
		{
			struct ll_400_graph* g = get_shm_ll_400_graph(file_name, vertices_count, edges_count);
			if(g != NULL)
			{
				assert(vertices_count == g->vertices_count);
				assert(edges_count == g->edges_count);

				print_ll_400_graph(g);
				*flags |= 1U<<31;
				return g;
			}
		}
		
	// Allocate memory
		struct ll_400_graph* g =calloc(sizeof(struct ll_404_graph),1);
		assert(g != NULL);
		g->vertices_count = vertices_count;
		g->edges_count = edges_count;
		g->offsets_list = numa_alloc_interleaved(sizeof(unsigned long) * (1 + g->vertices_count));
		assert(g->offsets_list != NULL);
		g->edges_list = numa_alloc_interleaved(sizeof(unsigned int) * g->edges_count);
		assert(g->edges_list != NULL);
		
	// Reading graph from disk
	{
		int fd=open(file_name, O_RDONLY | O_DIRECT);
		if(fd<0)
		{
			printf("Can't open the file: %d - %s\n",errno,strerror(errno));
			return NULL;
		}

		unsigned long buf_size=4096UL * 1024 * 24;
		char* buf=malloc(sizeof(char)*buf_size);
		assert(buf != NULL);
		char* main_buff_address = buf;
		
		// 4096 alignment of buf for O_DIRECT
		if((unsigned long)buf % 4096)
		{
			unsigned long excess = 4096 - (unsigned long)buf % 4096 ;
			buf = (char*)((unsigned long)buf + excess);
			buf_size -= 4096;
 		}

		long count=-1;

		unsigned long val=0;
		unsigned int val_length=0;
		unsigned int status=0;

		unsigned long t1=get_nano_time();
		unsigned long vl_count=0;
		unsigned long el_count=0;
		unsigned long total_read_bytes = 0;

		while((count=read(fd, buf, buf_size)) > 0)
		{
			total_read_bytes += count;
			int i=0;

			while(i<count)
			{
				//if(i<50) printf("status: %d, v: %c\n",status,buf[i]);
				if(buf[i]!='\n' && buf[i]!=' ')
				{
					val = val * 10 + (buf[i]-'0');
					val_length++;
				}
				else if(val_length)
				{
					switch(status)
					{
						case 0:
							assert(vertices_count == val);
							break;

						case 1:
							assert(edges_count == val);
							break;

						case 2:
							g->offsets_list[vl_count++]=val;
							break;

						case 3:
							assert(val < (1UL<<32));
							g->edges_list[el_count++]=val;
							break;
					}
					val=0;
					val_length=0;
				}
				
				if(buf[i] == '\n')
					status++;

				i++;
			}	
		}

		assert(count >= 0);
		assert(el_count == edges_count);
		assert(vl_count == vertices_count);

		free(main_buff_address);
		main_buff_address = NULL;
		buf = NULL;
		close(fd);
		fd = -1;

		g->offsets_list[g->vertices_count]=g->edges_count;

		printf("Reading %'.1f (MB) completed in %'.3f (seconds)\n", total_read_bytes/1e6, (get_nano_time() - t1)/1e9); 
	}

	// Printing the first vals in the read graph
		print_ll_400_graph(g);

	// Flush the OS cache
		flush_os_cache();

	*flags &= ~(1U<<31);

	return g;	
}


struct ll_404_graph* get_ll_404_txt_graph(char* file_name, unsigned int* flags)
{
	// Checks 
		assert(flags != NULL);

		if(access(file_name, F_OK) != 0)
		{
			printf("Error: file \"%s\" does not exist\n",file_name);
			return NULL;
		}

	// Reading vertices and edges count and graph size
		unsigned long vertices_count = 0;
		unsigned long edges_count = 0;	
		{
			char temp[512];
			sprintf(temp, "head -n1 %s", file_name);
			FILE *fp = popen(temp, "r");
			int ret = fscanf(fp, "%lu", &vertices_count);
			assert(ret == 1);
			pclose(fp);
			printf("Vertices: %'lu\n",vertices_count);

			sprintf(temp, "head -n2 %s | tail -n1", file_name);
			fp = popen(temp, "r");
			ret = fscanf(fp, "%lu", &edges_count);
			assert(ret == 1);
			pclose(fp);
			printf("Edges: %'lu\n",edges_count);
		}
		
	// Allocate memory
		struct ll_404_graph* g =calloc(sizeof(struct ll_404_graph),1);
		assert(g != NULL);
		g->vertices_count = vertices_count;
		g->edges_count = edges_count;
		g->offsets_list = numa_alloc_interleaved(sizeof(unsigned long) * (1 + g->vertices_count));
		assert(g->offsets_list != NULL);
		g->edges_list = numa_alloc_interleaved(sizeof(unsigned int) * g->edges_count);
		assert(g->edges_list != NULL);
		
	// Reading graph from disk
	{
		int fd=open(file_name, O_RDONLY | O_DIRECT);
		if(fd<0)
		{
			printf("Can't open the file: %d - %s\n",errno,strerror(errno));
			return NULL;
		}

		unsigned long buf_size=4096UL * 1024 * 24;
		char* buf=malloc(sizeof(char)*buf_size);
		assert(buf != NULL);
		char* main_buff_address = buf;
		
		// 4096 alignment of buf for O_DIRECT
		if((unsigned long)buf % 4096)
		{
			unsigned long excess = 4096 - (unsigned long)buf % 4096 ;
			buf = (char*)((unsigned long)buf + excess);
			buf_size -= 4096;
 		}

		long count=-1;

		unsigned long val=0;
		unsigned int val_length=0;
		unsigned int status=0;

		unsigned long t1=get_nano_time();
		unsigned long vl_count=0;
		unsigned long el_count=0;
		unsigned long total_read_bytes = 0;

		while((count=read(fd, buf, buf_size)) > 0)
		{
			total_read_bytes += count;
			int i=0;

			while(i<count)
			{
				//if(i<50) printf("status: %d, v: %c\n",status,buf[i]);
				if(buf[i]!='\n' && buf[i]!=' ')
				{
					val = val * 10 + (buf[i]-'0');
					val_length++;
				}
				else if(val_length)
				{
					switch(status)
					{
						case 0:
							assert(vertices_count == val);
							break;

						case 1:
							assert(edges_count == val);
							break;

						case 2:
							g->offsets_list[vl_count++]=val;
							break;

						case 3:
							assert(val < (1UL<<32));
							g->edges_list[el_count++]=val;
							break;
					}
					val=0;
					val_length=0;
				}
				
				if(buf[i] == '\n')
					status++;

				i++;
			}	
		}

		assert(count >= 0);
		printf("el_count %d\n", el_count);
		printf("edges_count %d\n", edges_count);
		printf("vl_count %d\n", vl_count);
		printf("vertices_count %d\n", vertices_count);
		assert(el_count == edges_count*2);
		assert(vl_count == vertices_count);

		free(main_buff_address);
		main_buff_address = NULL;
		buf = NULL;
		close(fd);
		fd = -1;

		g->offsets_list[g->vertices_count]=g->edges_count;

		printf("Reading %'.1f (MB) completed in %'.3f (seconds)\n", total_read_bytes/1e6, (get_nano_time() - t1)/1e9); 
	}

	// Printing the first vals in the read graph
		print_ll_400_graph((struct ll_400_graph*)g);

	// Flush the OS cache
		flush_os_cache();

	*flags &= ~(1U<<31);

	return g;	
}

/*
	`flags`:
		bit 0: 
			Read from storage. Do not use the copy in /dev/shm (if it exists).
		
		bit 31: 
			Will be set by the function if the graph has been mapped from a copy in /dev/shm. 
			This flag should be used for releasing the graph. If it is set, `release_shm_ll_400_graph()` should be called.
			Otherwise, `release_numa_interleaved_ll_400_graph()` should be called.
*/



struct ll_404_graph* get_shm_ll_404_graph(char* file_name, unsigned long vertices_count, unsigned long edges_count)
{
	char* shm_name = get_shm_graph_name(file_name);
	printf("shm_name: %s\n", shm_name);
	unsigned long graph_size = (2 + vertices_count + 1) * sizeof(unsigned long) + 2UL * edges_count * sizeof(unsigned int);

	struct ll_404_graph* g = NULL;
	int shm_fd = shm_open(shm_name, O_RDONLY, 0);
	if(shm_fd > 0)
	{
		printf("Shared memory file exists.\n");
		unsigned long* ul_graph = (unsigned long*)mmap(NULL, graph_size, PROT_READ, MAP_PRIVATE, shm_fd, 0);
		if(ul_graph == MAP_FAILED)
		{
			printf("Couldn't get graph -> mmap error : %d, %s\n", errno, strerror(errno) );
			assert (ul_graph != MAP_FAILED);
		}
		close(shm_fd);
		shm_fd = -1;
		
		g = malloc(sizeof(struct ll_404_graph));
		assert(g != NULL);
		g->vertices_count = ul_graph[0];
		g->edges_count = ul_graph[1];
		g->offsets_list = &ul_graph[2];
		g->edges_list = (unsigned int*)(&ul_graph[ 2 + ul_graph[0] + 1 ]);
	}

	// Release mem
		free(shm_name);
		shm_name = NULL;

	return g;
}


/*
	`flags`:
		bit 0: 
			Read from storage. Do not use the copy in /dev/shm (if it exists).
		
		bit 31: 
			Will be set by the function if the graph has been mapped from a copy in /dev/shm. 
			This flag should be used for releasing the graph. If it is set, `release_shm_ll_404_graph()` should be called.
			Otherwise, `release_numa_interleaved_ll_404_graph()` should be called.
*/

/*
	flags:
		bit 0: Directly use `file_name` without calling `get_shm_graph_name()`.
*/
int store_shm_ll_400_graph(struct par_env* pe, char* file_name, struct ll_400_graph* g, unsigned int flags)
{
	assert(file_name != NULL && g != NULL);

	int ret = -1;
	char* shm_name;
	if(flags & 1U)
		shm_name = file_name;
	else
		shm_name = get_shm_graph_name(file_name);
	unsigned long graph_size = (2 + g->vertices_count + 1) * sizeof(unsigned long) + g->edges_count * sizeof(unsigned int);
	
	unsigned long* sg = create_shm(shm_name, graph_size);
	if(sg != NULL)
	{
		numa_interleave_allocated_memory(sg, graph_size);

		sg[0] = g->vertices_count;
		sg[1] = g->edges_count;

		#pragma omp parallel for 
		for(unsigned int v=0; v <= g->vertices_count; v++)
			sg[2 + v] = g->offsets_list[v];

		unsigned int* sg_edges =(unsigned int*)(sg + 2 + g->vertices_count + 1);
		#pragma omp parallel for 
		for(unsigned long e=0; e < g->edges_count; e++)
			sg_edges[e] = g->edges_list[e];

		int ret = msync(sg, graph_size, MS_SYNC);
		assert(ret == 0);

		munmap(sg, graph_size);
		sg = NULL;

		{
			int shm_fd = shm_open(shm_name, O_RDONLY, 0);
			int r0 = fchmod(shm_fd, S_IRUSR|S_IRGRP|S_IROTH);
			assert(r0 == 0);
		
			close(shm_fd);
			shm_fd = -1;
		}

		ret = 0;
	}

	if(shm_name != file_name)
	{
		free(shm_name);
		shm_name = NULL;
	}

	return ret;
}

void delete_shm_graph_from(char* file_name)
{
	assert(file_name != NULL);

	char* shm_name = get_shm_graph_name(file_name);
	int ret = shm_unlink(shm_name);
	assert(ret == 0);

	free(shm_name);
	shm_name = NULL;

	return;
}

void release_numa_interleaved_ll_400_graph(struct ll_400_graph* g)
{
	assert(g!= NULL && g->offsets_list != NULL);

	numa_free(g->offsets_list, sizeof(unsigned long)*(1 + g->vertices_count));
	g->offsets_list = NULL;

	if(g->edges_list)
	{
		numa_free(g->edges_list, sizeof(unsigned int) * g->edges_count);
		g->edges_list = NULL;
	}

	free(g);
	g = NULL;

	return;
}

void release_shm_ll_400_graph(struct ll_400_graph* g)
{
	assert(g != NULL);
	assert( (void*)(g->offsets_list - 2) == (void*)(g->edges_list - 2 * (2 + g->vertices_count + 1)) );

	unsigned long graph_size = (2 + g->vertices_count + 1) * sizeof(unsigned long) + g->edges_count * sizeof(unsigned int);
	munmap(g->offsets_list - 2, graph_size);

	g->offsets_list = NULL;
	g->edges_list = NULL;

	free(g);
	g = NULL;

	return;
}

void release_numa_interleaved_ll_800_graph(struct ll_800_graph* g)
{
	assert(g!= NULL && g->offsets_list != NULL);

	numa_free(g->offsets_list, sizeof(unsigned long)*(1 + g->vertices_count));
	g->offsets_list = NULL;

	if(g->edges_list)
	{
		numa_free(g->edges_list, sizeof(unsigned long) * g->edges_count);
		g->edges_list = NULL;
	}

	free(g);
	g = NULL;

	return;
}

void release_numa_interleaved_ll_404_graph(struct ll_404_graph* g)
{
	assert(g!= NULL && g->offsets_list != NULL);

	numa_free(g->offsets_list, sizeof(unsigned long)*(1 + g->vertices_count));
	g->offsets_list = NULL;

	if(g->edges_list)
	{
		numa_free(g->edges_list, 2UL * sizeof(unsigned int) * g->edges_count);
		g->edges_list = NULL;
	}

	free(g);
	g = NULL;

	return;
}

void release_shm_ll_404_graph(struct ll_404_graph* g)
{
	assert(g != NULL);
	assert( (void*)(g->offsets_list - 2) == (void*)(g->edges_list - 2 * (2 + g->vertices_count + 1)) );

	unsigned long graph_size = (2 + g->vertices_count + 1) * sizeof(unsigned long) + 2 * g->edges_count * sizeof(unsigned int);
	munmap(g->offsets_list - 2, graph_size);

	g->offsets_list = NULL;
	g->edges_list = NULL;

	free(g);
	g = NULL;

	return;
}

#endif
