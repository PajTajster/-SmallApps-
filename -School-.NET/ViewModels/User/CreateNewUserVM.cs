using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using UsersData.Models;

namespace UsersData.ViewModels.User
{
    public class CreateNewUserVM : UserVM
    {
        CreateNewUserVM()
        {

        }
        public CreateNewUserVM(Models.User user) : base(user)
        {

        }
    }
}