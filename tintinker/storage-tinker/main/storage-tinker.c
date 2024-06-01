/* Finding Partitions Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <assert.h>
#include <string.h>

#include "esp_log.h"
#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"

static const char *TAG = "tinker";

void list_partitions(void){
  ESP_LOGI(TAG, "Iterating through partitions...");

  esp_partition_iterator_t it;
  it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);

  for (; it != NULL; it = esp_partition_next(it)) {
    const esp_partition_t *part = esp_partition_get(it);
    ESP_LOGI(TAG,
             "\tpartition <%d | %d | %s> start at offset 0x%" PRIx32
             " and has a size of 0x%" PRIx32,
             part->type , part->subtype,  part->label, part->address,  part->size);
  }

  // Release the partition iterator to release memory allocated for it
  esp_partition_iterator_release(it);
}

void app_main(void) {
  list_partitions();

  // Find the partition map in the partition table
  const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
  assert(partition != NULL);

  // Read entire partition to see if there is any data
  static char read_all_data[32];
  const esp_err_t err  = esp_partition_read(partition, 0, read_all_data, sizeof(read_all_data));
  ESP_LOGI(TAG, "Error code : %d",err);
  ESP_ERROR_CHECK(err);
  ESP_LOGI(TAG, "Read data: %s", read_all_data);

  static char store_data[] = "BAD APPLE! ~~ https://www.youtube.com/watch?v=9Xz4NV0zsbY";
  static char read_data[sizeof(store_data)];

  // Erase entire partition
  memset(read_data, 0xFF, sizeof(read_data));
  ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

  // Write the data, starting from the beginning of the partition
  ESP_ERROR_CHECK(esp_partition_write(partition, 0, store_data, sizeof(store_data)));
  ESP_LOGI(TAG, "Written data: %s", store_data);

  // Read back the data, checking that read data and written data match
  ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, sizeof(read_data)));
  assert(memcmp(store_data, read_data, sizeof(read_data)) == 0);
  ESP_LOGI(TAG, "Read data: %s", read_data);

  // Erase the area where the data was written. Erase size should be a multiple of SPI_FLASH_SEC_SIZE
  // and also be SPI_FLASH_SEC_SIZE aligned
  ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, SPI_FLASH_SEC_SIZE));

  // Read back the data (should all now be 0xFF's)
  memset(store_data, 0xFF, sizeof(read_data));
  ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, sizeof(read_data)));
  assert(memcmp(store_data, read_data, sizeof(read_data)) == 0);

  ESP_LOGI(TAG, "Erased data");

  ESP_LOGI(TAG, "Example end");
}
