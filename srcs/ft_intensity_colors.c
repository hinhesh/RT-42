/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_intensity_colors.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/10 15:30:22 by user42            #+#    #+#             */
/*   Updated: 2021/04/01 11:49:08 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mlx.h"
#include "function_maths.h"
#include "get_next_line.h"
#include "libft.h"
#include "parsing.h"
#include "image.h"

static t_list	*ft_initialize_color_2(t_scene *scene, double *t_min)
{
	*t_min = -1;
	return (*(scene->list));
}

t_list			*ft_color_intensity_2(t_scene *scene, t_ray *ray, t_point *base)
{
	t_list	*tmp;
	double	t_min;
	t_list	*min;

	tmp = ft_initialize_color_2(scene, &t_min);
	while (tmp != NULL)
	{
		if (tmp->type == 0 &&
intersection_sphere((t_sphere *)tmp->object, ray, base, &t_min) == 1)
			min = tmp;
		if (tmp->type == 1 &&
intersection_plan((t_plan *)tmp->object, ray, base, &t_min) == 1)
			min = tmp;
		if (tmp->type == 2 &&
intersection_square((t_square *)tmp->object, ray, base, &t_min) == 1)
			min = tmp;
		if (tmp->type == 3 &&
ft_intersection_triangle((t_triangle *)tmp->object, ray, base, &t_min) == 1)
			min = tmp;
		if (tmp->type == 4 &&
ft_intersection_cylinder((t_cylinder*)tmp->object, ray, base, &t_min) == 1)
			min = tmp;
		tmp = tmp->next;
	}
	return (min);
}

static void		ft_calculate_color(t_palette *color
, t_scene *scene, t_point base, t_light *tmp)
{
	double red;
	double green;
	double blue;

	red = (base.rgb.r * 0.2) + (tmp->rgb.r * 0.9);
	red += (scene->amb_light.rgb.r * 0.1);
	color->rgb.r += red * (color->intensity + (scene->amb_light.ratio * 0.1));
	green = ((base.rgb.g * 0.2) + (tmp->rgb.g * 0.9));
	green += (scene->amb_light.rgb.g * 0.1);
	color->rgb.g += green * (color->intensity + (scene->amb_light.ratio * 0.1));
	blue = ((base.rgb.b * 0.2) + (tmp->rgb.b * 0.9));
	blue += (scene->amb_light.rgb.b * 0.1);
	color->rgb.b += blue * (color->intensity + (scene->amb_light.ratio * 0.1));
}

static void		ft_color_intensity_3(t_palette *color
, t_scene *scene, t_point base, t_light *tmp)
{
	double	dist;
	t_coord	l;

	ft_vectors_substract(&base.pos, &tmp->pos, &l);
	dist = ft_norm2(&l);
	ft_normalize(&l);
	color->intensity += (ft_ombre(&base, dist, scene, tmp) *
	(ft_scal_produce(&l, &base.normal) +
	ft_specular(&base, &l, *(scene->camera)))
	/ dist) * tmp->i * 1000;
	ft_calculate_color(color, scene, base, tmp);
}

double			ft_color_intensity(t_palette *color, t_scene *scene,
t_ray *ray)
{
	t_point	base;
	t_list	*obj;
	t_light	*tmp;

	if (scene->nb_rebond != 0 &&
(obj = ft_color_intensity_2(scene, ray, &base)) != NULL)
	{
		if (obj->magic == 1)
			ft_mirror(color, ray, &base, scene);
		else
		{
			tmp = *(scene->light);
			while (tmp != NULL)
			{
				if (obj->magic == 3)
					color->magic = 3;
				else
					color->magic = 0;
				ft_color_intensity_3(color, scene, base, tmp);
				tmp = tmp->next;
			}
			return (color->intensity);
		}
	}
	return (-1);
}
