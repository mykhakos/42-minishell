#include "../include/cmd.h"


t_cmd *new_cmd(void)
{
    t_cmd *new;

    new = (t_cmd *)malloc(sizeof(t_cmd));
    if (!new)
        return (NULL);
    new->exec_path = NULL;
    new->cmd_args = NULL;
    new->in_redir = NULL;
    new->in_filename = NULL;
    new->in_fd = -1;
    new->out_redir = NULL;
    new->out_filename = NULL;
    new->out_fd = -1;
    new->next = NULL;
    return (new);
}

void cmd_add_back(t_cmd **cmd_lst, t_cmd *new)
{
    t_cmd *tmp;

    if (!cmd_lst || !new)
        return ;
    if (!*cmd_lst)
    {
        *cmd_lst = new;
        return ;
    }
    tmp = *cmd_lst;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new;
}

static void cmd_free_node(t_cmd *cmd_node)
{
    int i;

    i = 0;
    if (cmd_node->cmd_args)
    {
        while (cmd_node->cmd_args[i])
            free(cmd_node->cmd_args[i++]);
        free(cmd_node->cmd_args);
    }
    if (cmd_node->exec_path)
        free(cmd_node->exec_path);
    if (cmd_node->in_redir)
        free(cmd_node->in_redir);
    if (cmd_node->in_filename)
        free(cmd_node->in_filename);
    if (cmd_node->out_redir)
        free(cmd_node->out_redir);
    if (cmd_node->out_filename)
        free(cmd_node->out_filename);
    if (cmd_node->in_fd > -1)
        close(cmd_node->in_fd);
    if (cmd_node->out_fd > -1)
        close(cmd_node->out_fd);
}

void cmd_free(t_cmd **cmd_lst)
{
    t_cmd *curr;

    if (!cmd_lst || !*cmd_lst)
        return ;
    while (*cmd_lst)
    {
        curr = *cmd_lst;
        *cmd_lst = (*cmd_lst)->next;
        cmd_free_node(curr);
        free(curr);
    }
    *cmd_lst = NULL;
}
