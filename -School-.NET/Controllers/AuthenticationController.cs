using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Security;
using UsersData.Models;
using UsersData.ViewModels.Authentication;

namespace UsersData.Controllers
{
    public class AuthenticationController : Controller
    {
        
        public ActionResult Login()
        {
            return View();
        }

        [HttpPost]
        public ActionResult Login(LoginVM LVM, string returnURL)
        {
            AppBL userLogin = new AppBL();
            if (userLogin.isValidUser(LVM.Username, LVM.Password))
            {
                FormsAuthentication.SetAuthCookie(LVM.Username, false);
                return RedirectToAction(returnURL);
            }
            ModelState.AddModelError("CredentialError", "Niepoprawna nazwa lub hasło użytkownika");
            return View("Login");
        }

        public ActionResult Logout()
        {
            FormsAuthentication.SignOut();
            return RedirectToAction("Index", "User");
        }
    }
}