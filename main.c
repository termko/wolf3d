/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ydavis <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/28 15:38:52 by ydavis            #+#    #+#             */
/*   Updated: 2019/06/29 00:56:40 by ydavis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

void	init_map(t_wf *wf)
{
	int i;
	int j;

	wf->map = (int**)malloc(sizeof(int*) * MAPL);
	i = 0;
	while (i < MAPL)
	{
		wf->map[i] = (int*)malloc(sizeof(int) * MAPL);
		j = 0;
		while (j < MAPL)
		{
			if (!i || !j || i == MAPL - 1 || j == MAPL - 1)
				wf->map[i][j] = 1;
			else
				wf->map[i][j] = 0;
			j++;
		}
		i++;
	}
}

void	init_player(t_wf *wf)
{
	wf->pl = (t_pl*)malloc(sizeof(t_pl));
	wf->pl->posx = 340;
	wf->pl->posy = 340;
	wf->pl->angle = 45;
	wf->pl->fov = 60;
	wf->lov = 100 * 64;
	wf->dist = ((double)wf->width / 2.0) / tan(degtorad(wf->pl->fov) / 2.0);
	wf->angw = wf->pl->fov / wf->width;
}

int		close_app(t_wf *wf)
{
	SDL_DestroyWindow(wf->sdl->win);
	SDL_Quit();
	return (0);
}

double	degtorad(double deg)
{
	return (deg * (M_PI / 180));
}

void	fill_col(t_wf *wf, int i, double dist)
{
	int j;
	int	height;
	int	tmp;

	height = SQLEN * wf->dist / dist;
	tmp = (wf->height - height) / 2;
	j = 0;
	while (j < wf->height)
	{
		if (j > tmp && j < tmp + height)
			wf->sdl->pix[i + wf->width * j] = 255;
		j++;
	}
}

double	check_y(t_wf *wf, double omega)
{
	int		check;
	double	x;
	double	y;
	double	disty;
	double	movey;

	check = 0;
	disty = MAPL * 600;
	if (omega == 270 || omega == 90 || omega == 180 || omega == 0)
        return (disty);
	if (omega < 90 || omega > 270)
    {
		x = floor(wf->pl->posx / 64) * 64 + 64;
		y = wf->pl->posy + (wf->pl->posx - x) * tan(degtorad(omega));
    }
	else
    {
		x = floor(wf->pl->posx / 64) * 64;
        y = wf->pl->posy + (wf->pl->posx - x) * tan(degtorad(omega));
        x--;
    }
	movey = tan(degtorad(omega)) * 64;
	while (!check &&
			x > 0 &&
			y > 0 &&
			pow(x - wf->pl->posx, 2) + pow(y - wf->pl->posy, 2) <
			pow(wf->lov, 2) &&
			x < MAPL * 64 &&
			y < MAPL * 64)
	{
		if (wf->map[(int)(x / 64)][(int)(y / 64)])
		{
			check = 1;
			break ;
		}
		if (omega < 90 || omega > 270)
			x += 64;
		else
			x -= 64;
		y += movey;
	}
	if (check == 1)
		disty = fabs(wf->pl->posx - x) / cos(degtorad(omega));// * cos(degtorad(omega - wf->pl->angle));
    //printf("disty = %f\n", disty);
    if (disty < 0)
        disty = -disty;
    //printf("disty = %f\n", disty);
	return (disty);
}

double  check_x(t_wf *wf, double omega)
{
    double  dist;
    int     check;
    double  x;
    double  y;
    double  xmove;
    double  ymove;

    check = 0;
    dist = MAPL * 600;
    if (omega == 90 || omega == 270 || omega == 180 || omega == 0)
        return (dist);
    if (omega > 180 || omega < 0)
    {
        y = floor(wf->pl->posy / 64) * 64 + 64;
        x = wf->pl->posx + (wf->pl->posy - y) / tan(degtorad(omega));
    }

    else
    {
        y = floor(wf->pl->posy / 64) * 64;
        x = wf->pl->posx + (wf->pl->posy - y) / tan(degtorad(omega));
        y--;
    }
    xmove = 64 / tan(degtorad(omega));
    while (!check &&
            x > 0 &&
            y > 0 &&
            pow(x - wf->pl->posx, 2) + pow(y - wf->pl->posy, 2) <
            pow(wf->lov, 2) &&
            x < MAPL * 64 &&
            y < MAPL * 64)
    {
        if (wf->map[(int)(x / 64)][(int)(y / 64)])
        {
            check = 1;
            break ;
        }
        if (omega > 180)
            y += 64;
        else
            y -= 64;
        x += xmove;
    }
    if (check)
        dist = fabs(wf->pl->posy - y) / sin(degtorad(omega));// * cos(degtorad(omega - wf->pl->angle));
   // printf("omega = %f    distx = %f    ", omega, dist);
    if (dist < 0)
        dist = -dist;
    //printf("omega = %f    distx = %f    ", omega, dist);
    return (dist);
}

void	test(t_wf *wf)
{
	double	omega;
	int		i;
	double	movex;
	double	distx;
	double	disty;

	i = 0;
	omega = wf->pl->angle + wf->pl->fov / 2;
	if (omega >= 360)
        omega -= 360;
	while (i < wf->width)
	{
	    distx = check_x(wf, omega);
	    disty = check_y(wf, omega);
	    fill_col(wf, i, (distx < disty ? distx : disty));
	    omega -= wf->angw;
	    if (omega < 0)
            omega += 360;
        i++;
	}
	    /*
		check = 0;
		if ((omega > 180 || omega < 0) && omega < 360)
			y = floor(wf->pl->posy / 64) * 64 + 64;
		else
			y = floor(wf->pl->posy / 64) * 64 - 1;
		x = wf->pl->posx + (wf->pl->posy - y) / tan(degtorad(omega));
		movex = 64 / tan(degtorad(omega));
		while (!check &&
				x > 0 &&
				y > 0 &&
				pow(x - wf->pl->posx, 2) + pow(y - wf->pl->posy, 2) <
				pow(wf->lov, 2) &&
				x < MAPL * 64 &&
				y < MAPL * 64)
		{
			if (wf->map[(int)x / 64][(int)y / 64])
			{
				check = 1;
				break ;
			}
			if (omega >= 180)
				y += 64;
			else
				y -= 64;
			x += movex;
		}
		if (check == 1)
		{
			distx = fabs(wf->pl->posx - x) / cos(degtorad(omega)) * cos(degtorad(omega - wf->pl->angle));
			/*
			fill_col(wf, i, distx, omega);
			disty = check_y(wf, omega);
			if (disty < 0)
				fill_col(wf, i, distx, omega);
			else
				fill_col(wf, i, (distx < disty && disty > 0 ? distx : disty), omega);
		}
		disty = check_y(wf, omega);
		if (disty > 0)
			fill_col(wf, i, (distx < disty && distx > 0 ? distx : disty), omega);
		else if (distx > 0)
			fill_col(wf, i, distx, omega);
		omega -= wf->angw;
		if (omega >= 360)
			omega -= 360;
		i++;
		*/

	/*
	while (i < wf->width)
	{
		check = 0;
		distx = 0.0;
		disty = 0.0;
		x = wf->pl->posx;
		y = wf->pl->posy;
		while (x > 0 && y > 0 && x < wf->lov && y < wf->lov)
		{
			if (!((int)x % 64) || !((int)y % 64))
			{
				if (wf->map[(int)x / 64][(int)y / 64])
				{
					printf("%f - %d ; %f - %d\n", x, (int)x, y, (int)y);
					check = (!((int)x % 64) ? 1 : 2);
					break ;
				}
			}
			x += cos(omega);
			y -= sin(omega);
		}
		if (check == 1)
			fill_col(wf, i, distx);
		if (check == 2)
			fill_col(wf, i, disty);
		i++;
		omega += wf->angw;
	}
	*/

	SDL_UpdateTexture(wf->sdl->txt, NULL, wf->sdl->pix, wf->width * sizeof(Uint32));
	SDL_RenderClear(wf->sdl->ren);
	SDL_RenderCopy(wf->sdl->ren, wf->sdl->txt, NULL, NULL);
	SDL_RenderPresent(wf->sdl->ren);
}

void	prepare_window(t_wf *wf)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	wf->sdl->win = SDL_CreateWindow("HEY THERE", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, wf->width, wf->height, 0);
	wf->sdl->ren = SDL_CreateRenderer(wf->sdl->win, -1, 0);
	wf->sdl->txt = SDL_CreateTexture(wf->sdl->ren,
			SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, wf->width, wf->height);
	wf->sdl->pix = (Uint32*)malloc(sizeof(Uint32) * (wf->width * wf->height));
	memset(wf->sdl->pix, 255, wf->width * wf->height * sizeof(Uint32));
	SDL_UpdateTexture(wf->sdl->txt, NULL, wf->sdl->pix, wf->width * sizeof(Uint32));
	SDL_RenderClear(wf->sdl->ren);
	SDL_RenderCopy(wf->sdl->ren, wf->sdl->txt, NULL, NULL);
	SDL_RenderPresent(wf->sdl->ren);
}

int main(int ac, char **av)
{
	t_wf		*wf;
	SDL_Event	evt;

	wf = (t_wf*)malloc(sizeof(t_wf));
	wf->width = 1024;
	wf->height = 768;

	init_map(wf);
	init_player(wf);

	wf->sdl = (t_sdl*)malloc(sizeof(t_sdl));
	prepare_window(wf);

	test(wf);

	while (1)
	{
		while (SDL_PollEvent(&evt))
		{
			if ((SDL_QUIT == evt.type) ||
					(SDL_KEYDOWN == evt.type &&
					 SDL_SCANCODE_ESCAPE == evt.key.keysym.scancode))
				return (close_app(wf));
			if (SDL_KEYDOWN == evt.type &&
					SDLK_LEFT == evt.key.keysym.sym)
			{
				memset(wf->sdl->pix, 255, wf->width * wf->height * sizeof(Uint32));
				wf->pl->angle += 2;
				if (wf->pl->angle >= 360)
					wf->pl->angle -= 360;
				test(wf);
				printf("ANGLE IS %f, OMEGA IS [%f - %f]\n", wf->pl->angle, wf->pl->angle + wf->pl->fov / 2, wf->pl->angle - wf->pl->fov / 2);
			}
			if (SDL_KEYDOWN == evt.type &&
					SDLK_RIGHT == evt.key.keysym.sym)
			{
				wf->pl->angle -= 2;
				if (wf->pl->angle <= 0)
					wf->pl->angle += 360;
				memset(wf->sdl->pix, 255, wf->width * wf->height * sizeof(Uint32));
				test(wf);
				printf("ANGLE IS %f, OMEGA IS [%f - %f]\n", wf->pl->angle, wf->pl->angle + wf->pl->fov / 2, wf->pl->angle - wf->pl->fov / 2);
			}
			if (SDL_KEYDOWN == evt.type &&
					SDLK_UP == evt.key.keysym.sym)
			{
				memset(wf->sdl->pix, 255, wf->width * wf->height * sizeof(Uint32));
				wf->pl->posx += cos(degtorad(wf->pl->angle)) * 3;
				wf->pl->posy -= sin(degtorad(wf->pl->angle)) * 3;
				test(wf);
				printf("NEW POSX %f NEW POSY %f\n", wf->pl->posx, wf->pl->posy);
			}
			if (SDL_KEYDOWN == evt.type &&
					SDLK_DOWN == evt.key.keysym.sym)
			{
				memset(wf->sdl->pix, 255, wf->width * wf->height * sizeof(Uint32));
				wf->pl->posx -= cos(degtorad(wf->pl->angle)) * 3;
				wf->pl->posy += sin(degtorad(wf->pl->angle)) * 3;
				test(wf);
				printf("NEW POSX %f NEW POSY %f\n", wf->pl->posx, wf->pl->posy);
			}
		}
	}
	return (0);
}