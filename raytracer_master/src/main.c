/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abiri <abiri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/12 08:48:50 by abiri             #+#    #+#             */
/*   Updated: 2020/02/14 14:53:03 by abiri            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "master_raytracer.h"

int	ft_init_window(t_mlx *mlx)
{
	mlx->mlx_ptr = mlx_init();
	if (!(mlx->img.img_ptr = mlx_new_image(mlx->mlx_ptr,
			WIDTH, HEIGHT)))
		return (0);
	if (!(mlx->img.data = (int*)mlx_get_data_addr(mlx->img.img_ptr,
			&mlx->img.bpp, &mlx->img.size_l, &mlx->img.endian)))
		return (0);
	if (!(mlx->win = mlx_new_window(mlx->mlx_ptr, WIDTH,
		HEIGHT, "RAYTRACER\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tMASTER")))
		return (0);
	mlx->img.height = HEIGHT;
	mlx->img.width = WIDTH;
	return (1);
}

int	ft_send_textures(t_cluster *cluster)
{
	DIR				*directory;
	struct dirent	*file;
	t_packet		request;
	char			padding[17];
	char			*tex_path;
	char			*file_data;

	if (!(directory = opendir(TEXTURES_DIR)))
		return (0);
	request.type = TYPE_T_REQUEST_TEXTURE;
	request.data = NULL;
	while (!(file = readdir(directory)))
	{
		request.size = 0;
		if (file->d_type & O_DIRECTORY)
			continue ;
		ft_bzero(padding, 17);
		ft_strncpy(padding, file->d_name, 16);
		tex_path = ft_strjoin(TEXTURES_DIR, file->d_name);
		if (!(file_data = read_file(open(tex_path, O_RDONLY), &request.size)))
			return (0);
		request.size += 16;
		if (!(request.data = malloc(request.size)))
			return (0);
		ft_memcpy(request.data, padding, 16);
		ft_memcpy(request.data + 16, file_data, request.size - 16);
		free(tex_path);
		free(file_data);
		queue_task(cluster, create_packet(request, NULL));
	}
	return (1);
}

int	ft_send_scene(t_cluster *cluster, char *filename)
{
	t_packet	packet;

	if (!(packet.data = read_file(open(filename, O_RDONLY), &packet.size)))
		return (0);
	packet.type = TYPE_T_REQUEST_SCENE;
	queue_task(cluster, create_packet(packet, NULL));
	return (1);
}

int	ft_send_tasks(t_cluster *cluster)
{
	int			index;
	int			*height_interval;
	t_packet	packet;

	index = 0;
	if (!(height_interval = malloc(sizeof(int) * 2)))
		return (0);
	while (index < cluster->size)
	{
		height_interval[0] = (HEIGHT / cluster->size) * index;
		height_interval[1] = height_interval[0] + (HEIGHT / cluster->size);
		packet.data = height_interval;
		packet.size = sizeof(int) * 2;
		packet.type = TYPE_T_REQUEST_COMPUTATION;
		queue_task(cluster, create_packet(packet, NULL));
		index++;
	}
	return (1);
}

int	ft_display_loop(void *content)
{
	t_master_env *env;

	env = content;
	mlx_put_image_to_window(env->mlx.mlx_ptr,
		env->mlx.win, env->mlx.img.mlx_img, 0, 0);
	return (1);
}

int	ft_show_results(t_cluster *cluster, t_master_env *env)
{
	while (cluster->computation.tasks_queue.size != 0)
		continue ;
	env->mlx.img = fetch_computation_blob(cluster);
	return (1);
}

int main(int argc, char **argv)
{
	t_master_env	env;
	t_cluster		cluster;

	if (argc != 2)
		return (-1);
	if (!(ft_init_window(&env.mlx)))
		return (-1);
	if (!(init_cluster(argv[1], &cluster)))
		return (-1);
	if (!ft_send_textures(&cluster))
		return (-1);
	if (!ft_send_scene(&cluster, argv[1]))
		return (-1);
	if (!ft_send_tasks(&cluster))
		return (-1);
	if (!ft_show_results(&cluster, &env))
		return (-1);
	mlx_loop_hook(env.mlx.mlx_ptr, ft_display_loop, &env);
	mlx_loop(env.mlx.mlx_ptr);
	return (0);
}
