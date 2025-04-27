/*
 * Mã số sinh viên: 2310854
 * Copyright (C) 2025 pdnguyen của Đại học Bách Khoa HCM - Đại học Quốc gia TP.HCM
 */

/* Sierra release
 * Source Code License Grant: Tác giả cấp quyền cho Licensee
 * sử dụng và chỉnh sửa mã nguồn để phục vụ mục đích học tập
 * khi tham gia khóa học CO2018.
 */

#include "common.h"
#include "queue.h" // Bao gồm các hàm empty, enqueue, dequeue
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"
#include <string.h> // Bao gồm hàm strcmp để so sánh chuỗi
#include <stdlib.h> // Bao gồm hàm free để giải phóng bộ nhớ

/* Hàm __sys_killall: Dừng tất cả tiến trình có tên khớp với proc_name */
int __sys_killall(struct pcb_t *caller, struct sc_regs *regs)
{
    char proc_name[100]; // Tên của tiến trình cần dừng
    uint32_t data;
    uint32_t memrg = regs->a1; // ID của vùng nhớ chứa tên tiến trình

    /* Đọc tên tiến trình từ vùng nhớ */
    int i = 0;
    data = 0;
    while (data != -1) // Tiếp tục đọc cho đến khi gặp ký tự kết thúc (-1)
    {
        libread(caller, memrg, i, &data); // Đọc dữ liệu từ vùng nhớ
        proc_name[i] = data;
        if (data == -1)
            proc_name[i] = '\0'; // Kết thúc chuỗi nếu gặp -1
        i++;
    }
    printf("Tên tiến trình lấy từ memregionid %d là \"%s\"\n", memrg, proc_name);

    /* Lặp qua danh sách tiến trình đang chạy (running_list) */
    struct pcb_t *current_proc;
    while (!empty(caller->running_list)) // Kiểm tra danh sách không rỗng
    {
        current_proc = dequeue(caller->running_list);   // Lấy tiến trình đầu tiên
        if (strcmp(current_proc->path, proc_name) == 0) // So sánh tên tiến trình
        {
            printf("Dừng tiến trình: %s\n", current_proc->path);
            free(current_proc); // Giải phóng bộ nhớ của tiến trình
        }
        else
        {
            enqueue(caller->running_list, current_proc); // Đưa tiến trình trở lại danh sách
        }
    }

    /* Lặp qua mlq_ready_queue nếu bật bộ lập lịch MLQ */
#ifdef MLQ_SCHED
    for (int prio = 0; prio < MAX_PRIO; prio++) // Duyệt qua các mức ưu tiên
    {
        while (!empty(&caller->mlq_ready_queue[prio]))
        {
            struct pcb_t *current_mlq_proc = dequeue(&caller->mlq_ready_queue[prio]); // Lấy tiến trình
            if (strcmp(current_mlq_proc->path, proc_name) == 0)                       // So sánh tên tiến trình
            {
                printf("Dừng tiến trình MLQ: %s\n", current_mlq_proc->path);
                free(current_mlq_proc); // Giải phóng bộ nhớ của tiến trình
            }
            else
            {
                enqueue(&caller->mlq_ready_queue[prio], current_mlq_proc); // Đưa lại vào hàng đợi
            }
        }
    }
#endif

    return 0; // Trả về 0 sau khi hoàn thành
}

/*
 * Mã số sinh viên: 2310854
 */
