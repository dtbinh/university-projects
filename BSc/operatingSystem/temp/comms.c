#include "include/asm.h"
#include "include/error.h"
#include "include/stddef.h"
#include "include/mem.h"
#include "include/process.h"
#include "include/i386.h"
#include "include/io.h"
#include "include/comms.h"

static unsigned long comms_bitmap = 0xffffffff;
static str_msg_port *msg_port[COMMS_MAX_PORTS];

void block_process(unsigned int port,cpu_state* state)
{
  //get the running process
  process_list* runq = remove_running(state);
  
  //is it the first blocked process?
  if (msg_port[port]->bq == NULL)
  {
    //add to start of list
    msg_port[port]->bq = runq;    
  }else
  {
    //point currently last to runq
    msg_port[port]->bq_last->bq_next = runq;
  }
  //it is the last node
  msg_port[port]->bq_last = runq;
  //and doesn't have a next
  msg_port[port]->bq_last->bq_next = NULL;    
}



void unblock_process(unsigned int port)
{
  //is there blocked processes?
  if (msg_port[port]->bq != NULL)
  {
    //add the first process back into the ready queue
    add_to_rq(msg_port[port]->bq);
    //move onto next blocked process
    msg_port[port]->bq = msg_port[port]->bq->bq_next;  
    //make sure last points correctly if neccessary
    if (msg_port[port]->bq == NULL)
    {
      msg_port[port]->bq_last = NULL;
    }
  }
}

void update_error(unsigned int port,int error)
{
  
  while (msg_port[port]->bq != NULL)
  {
    msg_port[port]->bq->pcb->state->eax = error;
    add_to_rq(msg_port[port]->bq);
    msg_port[port]->bq = msg_port[port]->bq->bq_next; 
  }
  msg* temp = msg_port[port]->queue;
  while (temp != NULL)
  {
    msg_port[port]->queue = msg_port[port]->queue->next;
    mem_free(temp);
    temp = msg_port[port]->queue;
  }
}

void block_server(unsigned int port,cpu_state* state)
{
  //get the running process
  process_list* runq = remove_running(state);
  
  //is it the first blocked process?
  msg_port[port]->server = runq;    
  printk("blocking %x\n",runq);
}

int unblock_server(unsigned int port)
{
  //is there blocked processes?
  if (msg_port[port]->server != NULL)
  {//printk("hallo jy is blocked\n");
    //add the first process back into the ready queue
    add_to_rq(msg_port[port]->server);
    //move onto next blocked process
    printk("dis die server\n");
    return 1;
  }else
  {printk("dis nie die server nie\n");
  }
  return 0;
}
int comms_lookup_port(unsigned int port)
{
  //return invalid port when port exceeds max ports
  if (port >= COMMS_MAX_PORTS)
  {
    return -ERR_IPC_INVALID_PORT;
  }
  //return no such port when the port doesn't exist yet
  if (bt(comms_bitmap,port))
  {
    return -ERR_IPC_NO_SUCH_PORT;
  }
  
  //return OK when requested port could be located  
  return OK;
}

int comms_create_port(unsigned int port)
{
  //return duplicate port when the port exists
  if (bt(comms_bitmap,port) == 0)
  {
    return -ERR_IPC_DUPLICATE_PORT;
  }
  //return invalid port if port exceeds max ports
  if (port >= COMMS_MAX_PORTS)
  {
    return -ERR_IPC_INVALID_PORT;
  }
  
  //allocate memory for a struct for the port
  msg_port[port] = (struct str_msg_port*)mem_alloc(sizeof(struct str_msg_port));
  //there will be no messages now
  msg_port[port]->queue = NULL;
  msg_port[port]->last = NULL;
  //there isn't a blocked process
  msg_port[port]->bq = NULL;
  msg_port[port]->bq_last = NULL;
  msg_port[port]->server = NULL;
  //save the pcb of the server
  msg_port[port]->pcb = cr3();
  //get the index in the page directory. this will be the same for all processes
  msg_port[port]->index = msg_port[port]->pcb->base >> 22;
  //set port as allocated
  btr(comms_bitmap,port);
  
  //return OK
  return OK;
}

int comms_send(cpu_state* state)
{
  unsigned int port = state->ebx;
  void *req_buf = (void*)state->ecx;
  unsigned int req_size = state->edx;
  void *rep_buf = (void*)state->esi;
  unsigned int rep_size = state->edi;
  
  //return no such port when if the port doesn't exist
  if (bt(comms_bitmap,port))
  {
    state->eax = -ERR_IPC_NO_SUCH_PORT;
    return 0;
  }
  //return invalid port if port exceeds max ports
  if (port >= COMMS_MAX_PORTS)
  {
    state->eax = -ERR_IPC_INVALID_PORT;
    return 0;
  }
  //return invalid msg size if req_size or rep_size is less than 1 or exceeds max msg size
  if ((req_size < 1) | (req_size >= COMMS_MAX_MSG) | (rep_size < 1) | (rep_size >= COMMS_MAX_MSG))
  {
    state->eax = -ERR_IPC_INVALID_MSG_SIZE;
    return 0;
  }
  //return invalid buffer when a buffer equals NULL
  if ((req_buf == NULL) | (rep_buf == NULL))
  {
    state->eax = -ERR_IPC_INVALID_BUFFER;
    return 0;
  }
  
  //=========================================SEND MSG=====================================
  //block the client
  state->eax = OK;
  block_process(port,state);
  
  //allocate memory for the message
  msg *temp = (struct msg*)mem_alloc(sizeof(struct msg));
  //printk("temp = %x\n",temp);
  //is this the first message?
  if (msg_port[port]->queue == NULL)
  {
    //set as first node
    msg_port[port]->queue = temp;    
  }else
  {
    //let the last message point to it
    msg_port[port]->last->next = temp;    
  }
  //the msg is also the last msg
  msg_port[port]->last = temp;
  //save it's page directory
  
  temp->pd = cr3();
  printk("pd = %x\n",temp->pd);
  
  //save the adress of the request and it's size
  temp->req_buf = req_buf;
  temp->req_size = req_size;
  //save the adress we need to reply to and it's size
  temp->rep_buf = rep_buf;
  temp->rep_size = rep_size;
  //there isn't a next
  temp->next = NULL;
  //get the server page directory
  page_directory* pd = (page_directory*)(msg_port[port]->pcb->pd);
  
  //get the page table entry where the server is located
  unsigned int pt_entry = pd->table[msg_port[port]->index];
  
  //get the client page directory   
  pd = (page_directory*)(temp->pd);
  //get a open slot in the page directory 
  temp->offset = 0;
  while ((temp->offset < PTES) && (pd->table[temp->offset] != 0))
  {
    temp->offset++;
  }
    
  //add the server page table into the client's page directory
  pd->table[temp->offset] = pt_entry;
  
  //save the multiplier to work with the server adresses in the client pd
  temp->mult = (temp->offset - msg_port[port]->index) * 0x400000;
  
  //unblock the server if it's blocked
  if (unblock_server(port))
  {
    void *buf = (void*)msg_port[port]->pcb->state->ecx;
    //copy the message request into buf
    printk("Along came Mary\n");
    copy_4(temp->req_buf,buf+temp->mult,req_size/4);    
  }
  
  
  
  //return res
  return 1;    
}

int comms_receive(cpu_state* state)
{
  unsigned int port= state->ebx;
  void *buf = (void*)state->ecx;
  unsigned int size = state->edx;
  //return no such port when if the port doesn't exist
  if (bt(comms_bitmap,port))
  {
    state->eax = -ERR_IPC_NO_SUCH_PORT;
    return 0;
  }
  //return invalid port if port exceeds max ports
  if (port >= COMMS_MAX_PORTS)
  {
    state->eax = -ERR_IPC_INVALID_PORT;
    return 0;
  }
  //return invalid msg size if buf is less than 1 or exceeds max msg size
  if ((size < 1) | (size >= COMMS_MAX_MSG))
  {
    state->eax = -ERR_IPC_INVALID_MSG_SIZE;
    return 0;
  }
  //return invalid buffer when a buffer equals NULL
  if (buf == NULL)
  {
    state->eax = -ERR_IPC_INVALID_BUFFER;
    return 0;
  }
  state->eax = OK;
  //is there a message?
  if (msg_port[port]->queue != NULL)
  {
    //yes
    //get the message into temp
    msg* temp = msg_port[port]->queue;
    
    //switch to client pd
    i386_set_page_directory(temp->pd);
    //copy the message request into buf
    copy_4(temp->req_buf,buf+temp->mult,size/4);
    //switch back to server pd
    i386_set_page_directory(msg_port[port]->pcb->pd);
    
    return 0;
  }else
  {
    //no
    //block the server
    block_server(port,state);
    return 1;
  }
  
  //return OK
  return 0;
}

int comms_reply(unsigned int port, void *buf, unsigned int size)
{
  //return no such port when port doesn't exist
  if (bt(comms_bitmap,port))
  {
    update_error(port,-ERR_IPC_NO_SUCH_PORT);
    return -ERR_IPC_NO_SUCH_PORT;
  }
  //return invalid port when port exceeds max ports
  if (port >= COMMS_MAX_PORTS)
  {
    update_error(port,-ERR_IPC_INVALID_PORT);
    return -ERR_IPC_INVALID_PORT;
  }
  //return invalid msg size if buf is less than 1 or exceeds max msg size
  if ((size < 1) | (size >= COMMS_MAX_MSG))
  {
    update_error(port,-ERR_IPC_INVALID_MSG_SIZE);
    return -ERR_IPC_INVALID_MSG_SIZE;
  }
  //return invalid buffer when a buffer equals NULL
  if (buf == NULL)
  {
    update_error(port,-ERR_IPC_INVALID_BUFFER);
    return -ERR_IPC_INVALID_BUFFER;
  }
  
  
  //double check if the message is still there
  if (msg_port[port]->queue != NULL)
  {
    //save message into temp
    msg* temp = msg_port[port]->queue;
    
    if (temp->rep_size != size)
    {
      return -ERR_IPC_REPLY_MISMATCH;
    }
        
    //move to next message
    msg_port[port]->queue = msg_port[port]->queue->next;
    //if there is no messages make sure last knows this
    if (msg_port[port] == NULL)
    {
      msg_port[port]->last = NULL;
    }
    
    //switch to client pd
    i386_set_page_directory(temp->pd);
    
    //copy buf into reply address
    copy_4((unsigned int)buf+temp->mult,temp->rep_buf,size/4);
    
    //switch to server pd
    i386_set_page_directory(msg_port[port]->pcb->pd);
    
    //remove the server page table from the message
    ((page_directory*)(temp->pd))->table[temp->offset] = 0;
    
    //free the memory the message used    
    mem_free(temp);
    
    //unblock the client
    unblock_process(port);
  }else
  {
    //will this happen?
  }
  
  //return OK  
  return OK;
}

int comms_close_port(unsigned int port)
{
  //return error no such port if port doesn't exist
  if (bt(comms_bitmap,port))
  {
    return -ERR_IPC_NO_SUCH_PORT;
  }
  //return invalid port if port < 0 or bigger than max ports
  if (port >= COMMS_MAX_PORTS)
  {
    return -ERR_IPC_INVALID_PORT;
  }
  
  update_error(port,-ERR_IPC_CLOSE_PORT);
  //remove outstanding messages
  /*msg* temp = msg_port[port]->queue;
  while (temp != NULL)
  {
    msg_port[port]->queue = msg_port[port]->queue->next;
    mem_free(temp);
    temp = msg_port[port]->queue;
  }*/
  
  //free the memory the port used
  mem_free(msg_port[port]);
  
  //set port as free
  bts(comms_bitmap,port);
  
  //return OK
  return OK;
}
