using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using UsersData.Models;

namespace UsersData.ViewModels.User
{
    public class DeleteUserVM : UserVM
    {
        public DeleteUserVM()
        {

        }
        public DeleteUserVM(Models.User user) : base(user)
        {

        }
    }
}