#include "utils.h"
#include "widgets.h"

#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "resources/volume-00.xbm"
#include "resources/volume-01.xbm"
#include "resources/volume-02.xbm"
#include "resources/volume-03.xbm"
#include "resources/volume-04.xbm"

// libs: alsa

#define vol_images 5

Pixmap volume[vol_images];

snd_mixer_t *mixer;
snd_mixer_elem_t* master;

#define alsa_check(callee, message) \
  { int err = callee; \
    if (err < 0) fatal(message, snd_strerror(err)); \
  }

static struct command_arg args[] =
{
  { "volcard", "Sound card to indicate", "default" },
  { "volchan", "Sound channel to indicate", "Master" },
  { "volact", "Call the specified binary when volume widget activated", "talsamixer" }
};

void alsa_handler(int fd, void* data)
{
  snd_mixer_handle_events(mixer);
}

static void on_init(struct context* context, struct list_entry** notifiers)
{
  volume[0] = img_init(volume_00); volume[1] = img_init(volume_01);
  volume[2] = img_init(volume_02); volume[3] = img_init(volume_03);
  volume[4] = img_init(volume_04);

  snd_mixer_selem_id_t* sid;

  alsa_check(snd_mixer_open(&mixer, 0), "Cannot open mixer");
  alsa_check(snd_mixer_attach(mixer, arg_value(args, alen(args), "volcard")), "Cannot attach mixer");
  alsa_check(snd_mixer_selem_register(mixer, NULL, NULL), "Cannot register selem");
  alsa_check(snd_mixer_load(mixer), "Cannot load mixer");

  snd_mixer_selem_id_alloca(&sid);
  if (!sid) fatal("Cannot allocate selem id", NULL);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, arg_value(args, alen(args), "volchan"));

  master = snd_mixer_find_selem(mixer, sid);
  if (!master) fatal("Cannot find master volume control", NULL);

  int count = snd_mixer_poll_descriptors_count(mixer);
  struct pollfd* pollfds = calloc(count, sizeof(struct pollfd));
  alsa_check(snd_mixer_poll_descriptors(mixer, pollfds, count), "Cannot get poll descriptors");

  for (int i = 0; i < count; ++i)
  {
    struct notifier* alsa_notifier = malloc(sizeof(struct notifier));
    alsa_notifier->fd = pollfds[i].fd;
    alsa_notifier->data = NULL;
    alsa_notifier->callback = &alsa_handler;
    list_add(notifiers, alsa_notifier);
  }

  free(pollfds);
}

static Pixmap on_refresh()
{
  long min, max, cur;
  snd_mixer_selem_get_playback_volume_range(master, &min, &max);
  snd_mixer_selem_get_playback_volume(master, 0, &cur);
  return volume[vol_images * cur / (max - min + 1)];
}

static void on_activate()
{
  detach(arg_value(args, alen(args), "volact"));
}

static void on_del(struct context* context)
{
}

/////////////////// Initialization code ///////////////////

static void __attribute__ ((constructor)) init()
{
  static struct widget_desc description =
  {
    alen(args),
    args,
    &on_init,
    &on_refresh,
    &on_activate,
    &on_del
  };

  add_widget(&description);
}
