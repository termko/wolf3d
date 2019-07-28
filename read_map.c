/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ydavis <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/28 19:33:23 by ydavis            #+#    #+#             */
/*   Updated: 2019/07/28 22:14:09 by ydavis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int		destroy_obj(void *data_wf, void *data_obj)
{
	t_wf	*wf;
	t_obj	*obj_to_destroy;
	t_obj	*obj;
	t_list	*objs;

	wf = (t_wf*)data_wf;
	obj_to_destroy = (t_obj*)data_obj;
	objs = wf->objects;
	while (objs != NULL)
	{
		obj = (t_obj*)(objs->content);
		if (obj->instance_index == obj_to_destroy->instance_index)
			obj->enabled = 0;
		objs = objs->next;
	}
	wf->flash = 0.7;
	return (0);
	//TODO - Score update
	//TODO - Visual feedback
}

int		create_obj(t_wf *wf, int x, int y, int type)
{
	t_obj *obj;

	obj = (t_obj*)malloc(sizeof(t_obj));
	obj->pos_map = new_v2(x, y);
	obj->pos_real = new_v2((obj->pos_map.x + 0.5f) * SQLEN, (obj->pos_map.y + 0.5f) * SQLEN);
	obj->instance_index = x + y * wf->map_size;
	obj->enabled = 1;
	if (type == 2)
	{
		obj->tx = TXT_BOX;
		obj->on_col = NULL;
		obj->passable = 0;
	}
	if (type == 3)
	{
		obj->tx = TXT_LAMP;
		obj->on_col = NULL;
		obj->passable = 1;
	}
	if (type == 4)
	{
		obj->tx = TXT_HEALTH;
		obj->on_col = destroy_obj;
		obj->passable = 1;
	}

	ft_lstadd(&wf->objects, ft_lstnew(obj, sizeof(t_obj)));
	return (1);
}

int		bind_map(t_wf *data, char *map)
{
	int i;
	int j;
	int n;
	int type;

	//Allocate array
	printf("Allocating map...\n");
	data->map = (int**)malloc(sizeof(int*) * data->map_size);
	i = 0;
	while (i < data->map_size)
	{
		data->map[i] = (int*)malloc(sizeof(int) * data->map_size);
		ft_bzero(data->map[i], sizeof(int) * data->map_size);
		i++;
	}
	//Bind corresponding values
	printf("Binding map values...\n");
	n = 0;
	i = 0;
	while (map[n])
	{
		j = 0;
		while (map[n] != '\n')
		{
			if (ft_isdigit(map[n]))
			{
				type = map[n] - '0';
				if (type > 1)
					create_obj(data, i, j, type);
				data->map[i][j] = type;
			}
			n++;
			j++;
		}
		while (j < data->map_size)
			data->map[i][j++] = 0;
		n++;
		i++;
	}
	printf("Objects detected on map: %d\n", ft_lstcount(data->objects));
	return (1);
}

int		read_map(t_wf *data, char *file_name)
{
	int		fd;
	char	*str;
	char	*map;
	t_v2	map_size;

	printf("Opening map...\n");
	fd = open(file_name, O_RDONLY);
	map = ft_strnew(0);
	map_size = new_v2(0, 0);
	if (fd <= 0)
	{
		printf("Error opening map!\n");
		return (0);
	}
	printf("Reading map...\n");
	while (get_next_line(fd, &str) > 0)
	{
		if (ft_strlen(str) > map_size.x)
		{
			//printf("prev %f, curr %d\n", map_size.x, (int)ft_strlen(str));
			map_size.x = ft_strlen(str);
		}
		map = ft_strjoinc(map, str);
		map = ft_strjoinc(map, "\n");
		ft_strdel(&str);
		map_size.y++;
	}
	printf("Read map:\n%s\n", map);
	printf("Binding map...\n");
	data->map_size = (int)max(map_size.x, map_size.y);
	bind_map(data, map);
	printf("Map clean up...\n");
	ft_strdel(&map);
	printf("Map read!\n");
	return (1);
}

int	*read_texture(char *file_name)
{
	int		n;
	char	*tmp;
	char	*str;
	int		i;
	int		j;
	int		*texture;

	n = open(file_name, O_RDONLY);
	if (n <= 0)
		return (NULL);
	str = ft_strnew(0);
	//printf("Reading texture...\n");
	while (get_next_line(n, &tmp) > 0)
	{
		str = ft_strjoinc(str, tmp);
		str = ft_strjoinc(str, "\n");
		ft_strdel(&tmp);
	}
	//printf("Texture read.\n");
	//printf("Opening texture...\n");
	texture = (int*)malloc(sizeof(int) * 32 * 32);
	i = 0;
	n = 0;
	while (str[n])
	{
		j = 0;
		while (str[n] && str[n] != '\n')
		{
			tmp = ft_strnew(0);
			while (isdigit(str[n]))
			{
				tmp = ft_strjoinc(tmp, &str[n]);
				n++;
			}
			texture[j + i * 32] = ft_atoi(tmp);
			ft_strdel(&tmp);
			n++;
			j++;
		}
		n++;
		i++;
	}
	printf("%s was loaded.\n", file_name);
	return (texture);
}
