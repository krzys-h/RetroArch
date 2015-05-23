/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2015 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#include <file/file_path.h>
#include <file/dir_list.h>
#include "../file_ext.h"
#include "../file_ops.h"

#include "../general.h"
#include "../runloop_data.h"
#include "tasks.h"


#ifdef HAVE_LIBRETRODB

#ifdef HAVE_ZLIB
static int zlib_compare_crc32(const char *name, const char *valid_exts,
      const uint8_t *cdata, unsigned cmode, uint32_t csize, uint32_t size,
      uint32_t crc32, void *userdata)
{
   RARCH_LOG("CRC32: 0x%x\n", crc32);

   return 1;
}
#endif
static int database_info_iterate_playlist(
      database_info_handle_t *db, const char *name)
{
   char parent_dir[PATH_MAX_LENGTH];

   path_parent_dir(parent_dir);

   if (!strcmp(path_get_extension(name), "zip"))
   {
#ifdef HAVE_ZLIB
      RARCH_LOG("[ZIP]: name: %s\n", name);

      if (!zlib_parse_file(name, NULL, zlib_compare_crc32,
               (void*)parent_dir))
         RARCH_LOG("Could not process ZIP file.\n");
#endif
   }
   else
   {
      char msg[PATH_MAX_LENGTH];
      ssize_t ret;
      uint32_t crc, target_crc = 0;
      uint8_t *ret_buf         = NULL;
      int read_from            = read_file(name, (void**)&ret_buf, &ret);

      (void)target_crc;

      if (read_from != 1 || ret <= 0)
         return 0;

      snprintf(msg, sizeof(msg), "%zu/%zu: Scanning %s...\n",
            db->list_ptr, db->list->size, name);

      rarch_main_msg_queue_push(msg, 1, 180, true);

#ifdef HAVE_ZLIB
      crc = zlib_crc32_calculate(ret_buf, ret);

      RARCH_LOG("CRC32: 0x%x .\n", (unsigned)crc);
#endif

      if (ret_buf)
         free(ret_buf);
   }

   db->status = DATABASE_STATUS_ITERATE_NEXT;

   return 0;
}

static int database_info_iterate_next(
      database_info_handle_t *db)
{
   db->list_ptr++;

   if (db->list_ptr < db->list->size)
      return 0;
   return -1;
}

static int database_info_iterate(database_info_handle_t *db)
{
   const char *name = NULL;

   if (!db || !db->list)
      return -1;

   name = db->list->elems[db->list_ptr].data;

   if (!name)
      return 0;

   switch (db->type)
   {
      case DATABASE_TYPE_NONE:
         break;
      case DATABASE_TYPE_ITERATE:
         return database_info_iterate_playlist(db, name);
         break;
   }

   return 0;
}

static int database_info_poll(db_handle_t *db)
{
   const char *path = msg_queue_pull(db->msg_queue);

   if (!path)
      return -1;

   db->handle = database_info_init("/home/squarepusher/roms", DATABASE_TYPE_ITERATE);

   return 0;
}

void rarch_main_data_db_iterate(bool is_thread, void *data)
{
   data_runloop_t         *runloop = (data_runloop_t*)data;
   database_info_handle_t      *db = runloop ? runloop->db.handle : NULL;

   if (!db || !runloop)
      goto do_poll;

   switch (db->status)
   {
      case DATABASE_STATUS_ITERATE:
         database_info_iterate(db);
         break;
      case DATABASE_STATUS_ITERATE_NEXT:
         if (database_info_iterate_next(db) == 0)
            db->status = DATABASE_STATUS_ITERATE;
         else
         {
            rarch_main_msg_queue_push("Scanning of directory finished.\n", 0, 180, true);
            db->status = DATABASE_STATUS_FREE;
         }
         break;
      case DATABASE_STATUS_FREE:
         database_info_free(db);
         if (runloop->db.handle)
            free(runloop->db.handle);
         runloop->db.handle = NULL;
         break;
      default:
      case DATABASE_STATUS_NONE:
         goto do_poll;
   }

   return;

do_poll:
   if (database_info_poll(&runloop->db) != -1)
   {
      if (runloop->db.handle)
         runloop->db.handle->status = DATABASE_STATUS_ITERATE;
   }
}
#endif
