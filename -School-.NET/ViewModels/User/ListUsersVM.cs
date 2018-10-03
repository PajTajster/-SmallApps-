using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using UsersData.Models;

namespace UsersData.ViewModels.User
{
    public class ListUsersVM : UserVM
    {
        public ListUsersVM()
        {

        }
        public ListUsersVM(Models.User user) : base(user)
        {

        }
    }
}